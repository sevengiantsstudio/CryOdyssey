#include "StdAfx.h"
#include "VehicleController.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>

#include <CryPhysics/physinterface.h>
#include <CryRenderer/IRenderAuxGeom.h>

class CPlugin_CryDefaultEntities;

#include <CryGame/IGameFramework.h>

#include <DefaultComponents/Physics/Vehicles/WheelComponent.h>
#include "Components/InterfaceComponent.h"

#include "Core/GameInput/GameInput.h"
#include "Core/GameEvents/GameEvents.h"

#include  <CrySchematyc/Utils/SharedString.h>

using namespace Game;

static void RegisterVehicleControllerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope entityScope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = entityScope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CVehicleController));

		// Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::SetSteeringAngle, "{F910AD94-10EC-4964-BFF4-1924D88846EF}"_cry_guid, "SetSteeringAngle");
			pFunction->BindInput(1, 'ang', "Angle");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::SetCurrentGear, "{366765C7-4AB4-459C-86FD-0AF805100C06}"_cry_guid, "SetCurrentGear");
			pFunction->SetDescription("Switches to the specified gear (-1 reverse, 0 neutral, 1 and above is forward)");
			pFunction->BindInput(1, 'gear', "Gear");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::GetCurrentGear, "{71592DE4-2F6A-4B22-A1DB-69378605707B}"_cry_guid, "GetCurrentGear");
			pFunction->SetDescription("Gets the currently active gear (-1 reverse, 0 neutral, 1 and above is forward)");
			pFunction->BindOutput(0, 'gear', "Gear");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::GearUp, "{6F6071A2-4541-4987-BBF5-5100A3188D07}"_cry_guid, "GearUp");
			pFunction->SetDescription("Switched to the next gear");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::GearDown, "{4A9FCA78-9B37-4D2A-A1CB-2C88E4B97B2C}"_cry_guid, "GearDown");
			pFunction->SetDescription("Switched to the previous gear");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CVehicleController::SpawnEntityAtJoint, "{83FEF660-A845-4EB4-9378-6671F243FCBF}"_cry_guid, "SpawnEntityAtJoint");
			pFunction->BindInput(2, 'clas', "Entity Class");
			pFunction->BindInput(3, 'tran', "JointName");
			pFunction->BindOutput(1, 'ent', "Entity");
			componentScope.Register(pFunction);
		}
		// Signals
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnFire);
			componentScope.Register(pSignal);
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterVehicleControllerComponent)

CVehicleController::CVehicleController()
{
	m_gearParams.m_gears.EmplaceBack(SGearParams::SGear{ float((int)EGear::Reverse) });
	m_gearParams.m_gears.EmplaceBack(SGearParams::SGear{ float((int)EGear::Neutral) });
	m_gearParams.m_gears.EmplaceBack(SGearParams::SGear{ float((int)EGear::Forward) });
}

CVehicleController::~CVehicleController()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_NONE;
	m_pEntity->Physicalize(physParams);
}

void CVehicleController::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();

	Reset();

	if (m_pCachedCharacter)
	{
		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

		int jointCount = rIDefaultSkeleton.GetJointCount();
		for (int i = 0; i < jointCount; i++)
		{
			string jointName = rIDefaultSkeleton.GetJointNameByID(i);

			if (jointName == "wheel_0" || jointName == "wheel_1" || jointName == "wheel_2" || jointName == "wheel_3")
			{
				if (IStatObj* pStatObj = pSkeleton->GetStatObjOnJoint(i))
				{
					AABB aabb = pStatObj->GetAABB();
					QuatT rel = pSkeleton->GetAbsJointByID(i);

					Cry::DefaultComponents::CWheelComponent* wheel = m_pEntity->CreateComponent<Cry::DefaultComponents::CWheelComponent>();
					if (i == 0 || i == 1)
						wheel->SetAxleIndex(0);
					if (i == 2 || i == 3)
						wheel->SetAxleIndex(1);
					wheel->m_radius = aabb.GetSize().z * 0.5f;
					wheel->m_height = aabb.GetSize().x;
					wheel->m_handBrake = false;
					wheel->m_driving = true;
					wheel->m_suspensionLength = 1.5f;
					wheel->m_suspensionLengthComp = 0.7f;

					wheel->m_pStatObj = pStatObj;

					wheel->m_surfaceTypeName = "wheel";

					wheel->SetTransformMatrix(Matrix34(rel));

					SWheelInfo wheelInfo;
					wheelInfo.m_boneId = i;
					wheelInfo.m_slotId = wheel->GetEntitySlotId();
					m_wheelsInfo.push_back(wheelInfo);
				}
			}
		}
	}

	Physicalize();

	SetupInteractHandleObjects();

	CryCreateClassInstanceForInterface(cryiidof<IAnimationOperatorQueue>(), m_poseWheelsModifier);
	CryCreateClassInstanceForInterface(cryiidof<IAnimationOperatorQueue>(), m_posePowerControlModifier);
	CryCreateClassInstanceForInterface(cryiidof<IAnimationOperatorQueue>(), m_poseSteeringControlModifier);
	CryCreateClassInstanceForInterface(cryiidof<IAnimationOperatorQueue>(), m_poseFireControlModifier);
}

Cry::Entity::EventFlags CVehicleController::GetEventMask() const
{
	return Cry::Entity::EEvent::Reset | Cry::Entity::EEvent::GameplayStarted | 
		Cry::Entity::EEvent::PrePhysicsUpdate |	Cry::Entity::EEvent::EditorPropertyChanged;
}

void CVehicleController::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	case Cry::Entity::EEvent::Reset:
	{
		if (gEnv->IsEditor())
		{
			if (event.nParam[0] == 1)
			{
				m_pInterfaceComponent->AddInterface<IVehicle>(this);
			}
			if (event.nParam[0] == 0)
			{
				m_powerControlValue = 0.0f;
				m_steeringControlValue = 0.0f;
			}
		}
		else if (!gEnv->IsEditor())
		{
			m_pInterfaceComponent->AddInterface<IVehicle>(this);
		}
	}
	break;
	case EEntityEvent::PrePhysicsUpdate:
	{
		Update();
		UpdateObjectInteraction();
	}
	break;
	case EEntityEvent::EditorPropertyChanged:
	{
	}
	break;
	}
}

void CVehicleController::StartInteraction(const Vec3& pt)
{
	m_currentInteractObjectId = FindClosestInteractiveObject(pt);
	if (m_currentInteractObjectId != -1)
	{
		m_bIsInteracted = true;
	}
}

void CVehicleController::StopInteraction()
{
	m_bIsInteracted = false;
	m_currentInteractObjectId = -1;
}

bool CVehicleController::GetClosestInteractiveEntity(const Vec3& pt)
{
	m_currentInteractObjectId = FindClosestInteractiveObject(pt);
	if (m_currentInteractObjectId != -1)
	{
		return true;
	}
	return false;
}


void CVehicleController::SetCurrentGear(int gearId)
{
	pe_action_drive driveAction;
	driveAction.iGear = gearId + 1;
	m_pEntity->GetPhysicalEntity()->Action(&driveAction);
}

void CVehicleController::SetThrottle(Schematyc::Range<0, 1> ratio)
{
	pe_action_drive driveAction;
	driveAction.pedal = ratio;
	m_pEntity->GetPhysicalEntity()->Action(&driveAction);
}

void CVehicleController::SetSteeringAngle(CryTransform::CAngle angle)
{
	pe_action_drive driveAction;
	driveAction.steer = angle.ToRadians();
	m_pEntity->GetPhysicalEntity()->Action(&driveAction);
}

void CVehicleController::UseHandbrake(bool bSet)
{
	pe_action_drive driveAction;
	driveAction.bHandBrake = bSet ? 1 : 0;
	m_pEntity->GetPhysicalEntity()->Action(&driveAction);
}

void CVehicleController::SetBrake(Schematyc::Range<0, 1> ratio)
{
	pe_action_drive driveAction;
	driveAction.pedal = -ratio;
	m_pEntity->GetPhysicalEntity()->Action(&driveAction);
}

void CVehicleController::SetVelocity(const Vec3& velocity)
{
	if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
	{
		pe_action_set_velocity action_set_velocity;
		action_set_velocity.v = velocity;
		pPhysicalEntity->Action(&action_set_velocity);
	}
}

Vec3 CVehicleController::GetVelocity() const
{
	if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
	{
		pe_status_dynamics dynStatus;
		if (pPhysicalEntity->GetStatus(&dynStatus))
		{
			return dynStatus.v;
		}
	}

	return ZERO;
}

void CVehicleController::SpawnEntityAtJoint(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint)
{
	ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
	IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

	int jointId = rIDefaultSkeleton.GetJointIDByName(joint.c_str());
	QuatT loc = QuatT(m_pEntity->GetWorldTM()) * pSkeleton->GetAbsJointByID(jointId);

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = className.value.size() > 0 ? gEnv->pEntitySystem->GetClassRegistry()->FindClass(className.value) : nullptr;
	spawnParams.vPosition = loc.t;
	spawnParams.vScale = Vec3(1,1,1);
	spawnParams.qRotation = loc.q;

	if (const IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		entityIdOut = Schematyc::ExplicitEntityId(pEntity->GetId());
	}
	else
	{
		entityIdOut = Schematyc::ExplicitEntityId(INVALID_ENTITYID);
	}
}


void CVehicleController::Reset()
{
	if (m_characterFilePath.value.size() > 0)
	{
		m_pCachedCharacter = gEnv->pCharacterManager->CreateInstance(m_characterFilePath.value);
	}
	else
	{
		m_pCachedCharacter = nullptr;
	}

	if (m_pCachedCharacter)
	{
		m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId(), false);
	}

	if (!m_materialPath.value.empty())
	{
		if (IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(m_materialPath.value, false))
		{
			m_pEntity->SetSlotMaterial(GetEntitySlotId(), pMaterial);
		}
	}

	m_pEntity->SetSlotFlags(GetEntitySlotId(), ENTITY_SLOT_RENDER);

	if (IRenderNode* pRenderNode = m_pEntity->GetSlotRenderNode(GetEntitySlotId()))
	{
		uint32 slotFlags = m_pEntity->GetSlotFlags(GetEntitySlotId());
		slotFlags |= ENTITY_SLOT_CAST_SHADOW;
		m_pEntity->SetSlotFlags(GetEntitySlotId(), slotFlags);

		m_pEntity->AddFlags(ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO);
		pRenderNode->SetViewDistRatio((int)floor((100.0f / 100.f) * 255));
		pRenderNode->SetLodRatio((int)floor(((float)100.0f / 100.f) * 255));
	}

	SEntityPhysicalizeParams physParams;
	physParams.type = PE_RIGID;
	physParams.nSlot = std::numeric_limits<int>::max();
	m_pEntity->Physicalize(physParams);
}

void CVehicleController::Physicalize()
{
	m_pEntity->UnphysicalizeSlot(GetEntitySlotId());

	m_pEntity->UpdateComponentEventMask(this);

	SEntityPhysicalizeParams physParams;
	physParams.type = PE_WHEELEDVEHICLE;

	pe_params_car carParams;
	carParams.enginePower = m_engineParams.m_power;
	carParams.engineMaxRPM = m_engineParams.m_maxRPM;
	carParams.engineMinRPM = m_engineParams.m_minRPM;
	carParams.engineIdleRPM = m_engineParams.m_idleRPM;
	carParams.engineStartRPM = m_engineParams.m_startRPM;

	carParams.nGears = m_gearParams.m_gears.Size();
	carParams.engineShiftUpRPM = m_gearParams.m_shiftUpRPM;
	carParams.engineShiftDownRPM = m_gearParams.m_shiftDownRPM;

	m_gearRatios.resize(carParams.nGears);
	for (auto it = m_gearRatios.begin(); it != m_gearRatios.end(); ++it)
	{
		uint32 index = (uint32)std::distance(m_gearRatios.begin(), it);
		*it = m_gearParams.m_gears.At(index).m_ratio;
	}

	carParams.gearRatios = m_gearRatios.data();

	carParams.wheelMassScale = 1.f;
	physParams.pCar = &carParams;

	// Don't physicalize a slot by default
	physParams.nSlot = std::numeric_limits<int>::max();
	m_pEntity->Physicalize(physParams);

	UseHandbrake(false);

	if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
	{
		SEntityPhysicalizeParams physParams;
		physParams.nSlot = GetEntitySlotId();
		physParams.type = pPhysicalEntity->GetType();
		physParams.nLod = 1;
		physParams.fStiffnessScale = 0;

		physParams.mass = 5000.f;
		physParams.density = -1.0f;

		m_pEntity->PhysicalizeSlot(GetEntitySlotId(), physParams);
	}

	//pe_params_pos pp;
	//pp.doubleBufCoords = true;
	//m_pEntity->GetPhysicalEntity()->SetParams(&pp);
}

void CVehicleController::Update()
{
	pe_status_vehicle vehicleStatus;
	m_pEntity->GetPhysicalEntity()->GetStatus(&vehicleStatus);


	// Debug
	//IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
	//debug->Begin("player_car", true);
	//debug->Add2DText(string().Format("engine rpm: %f", vehicleStatus.engineRPM).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
	//debug->Add2DText(string().Format("engine gear: %i", GetCurrentGear()).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
	//debug->Add2DText(string().Format("engine throttle: %f", vehicleStatus.pedal).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
	//debug->Add2DText(string().Format("brake: %f", vehicleStatus.footbrake).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
	//debug->Add2DText(string().Format("vel: %f", vehicleStatus.vel.len2()).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
	//~Debug

	if (m_pCachedCharacter)
	{
		m_pCachedCharacter->GetISkeletonAnim()->PushPoseModifier(2, m_poseWheelsModifier, "Pose Wheels");
		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();

		for (SWheelInfo wheel : m_wheelsInfo)
		{
			QuatT rel = pSkeleton->GetAbsJointByID(wheel.m_boneId);

			pe_status_wheel sw;
			pe_status_pos sp;
			sw.partid = wheel.m_slotId;
			sp.partid = wheel.m_slotId;

			m_pEntity->GetPhysicalEntity()->GetStatus(&sw);
			m_pEntity->GetPhysicalEntity()->GetStatus(&sp);

			rel.q = sp.q;
			rel.t = sp.pos;

			m_poseWheelsModifier->PushOrientation(wheel.m_boneId, IAnimationOperatorQueue::eOp_OverrideWorld, rel.q);
			m_poseWheelsModifier->PushPosition(wheel.m_boneId, IAnimationOperatorQueue::eOp_OverrideWorld, rel.t);
		}
	
	}
}

void CVehicleController::SetupInteractHandleObjects()
{
	if (m_pCachedCharacter)
	{
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();
		
		int jointCount = rIDefaultSkeleton.GetJointCount();
		for (int i = 0; i < jointCount; i++)
		{
			string jointName = rIDefaultSkeleton.GetJointNameByID(i);
			std::size_t handle = jointName.find("handle");
			if (handle != std::string::npos)
			{
				m_interactiveHandles.push_back(SInteractiveHandle(jointName));
			}

		}

	}
}

int CVehicleController::FindClosestInteractiveObject(const Vec3& pt)
{
	if (m_pCachedCharacter)
	{
		if (m_interactiveHandles.size() > 0)
		{
			ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
			IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

			const QuatT entityLoc(m_pEntity->GetWorldTM());

			for (SInteractiveHandle handle : m_interactiveHandles)
			{
				string handleName = handle.m_name;
				int handleId = rIDefaultSkeleton.GetJointIDByName(handleName);
				Vec3 handlePos = (entityLoc * pSkeleton->GetAbsJointByID(handleId)).t;

				float distance = handlePos.GetDistance(pt);

				if (distance < 0.1f)
				{
					string interactiveObjectName = handleName.erase(0, handleName.find_first_of("_") + 1);
					return rIDefaultSkeleton.GetJointIDByName(interactiveObjectName);
				}
			}
		}
	}

	return -1;
}

void CVehicleController::UpdateObjectInteraction()
{
	if (m_pCachedCharacter)
	{
		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

		int powerId = rIDefaultSkeleton.GetJointIDByName("power");
		int steeringId = rIDefaultSkeleton.GetJointIDByName("angle");
		int fireId = rIDefaultSkeleton.GetJointIDByName("fire");

		if (m_bIsBrake)
		{
			SetVelocity(Vec3::CreateLerp(GetVelocity(), ZERO, gEnv->pTimer->GetFrameTime()));
		}

		if (m_bIsInteracted)
		{
			if (m_currentInteractObjectId == powerId)
			{
				m_powerControlValue += CGameInput::GetInstance()->GetMouseDelta().y * gEnv->pTimer->GetFrameTime() * 15.0f;
				SetThrottle(m_powerControlValue / 90.0f);

				if (m_powerControlValue <= -15.0f)
				{
					m_powerControlValue = 0.0f;
					SetThrottle(0.0f);
					m_bIsBrake = true;
					UseHandbrake(true);
					m_bIsInteracted = false;
				}

				if (m_powerControlValue > 5.0f)
				{
					m_bIsBrake = false;
					UseHandbrake(false);
				}
			}
			if (m_currentInteractObjectId == steeringId)
			{
				m_steeringControlValue += CGameInput::GetInstance()->GetMouseDelta().x * gEnv->pTimer->GetFrameTime() * 9.0f;
				SetSteeringAngle(CryTransform::CAngle::FromDegrees(m_steeringControlValue * 0.5f));
			}
			if (m_currentInteractObjectId == fireId)
			{
				m_fireControlValue += CGameInput::GetInstance()->GetMouseDelta().y * gEnv->pTimer->GetFrameTime() * 15.0f;

				if (m_fireControlValue <= -35.0f)
				{
					if (Schematyc::IObject* pObject = m_pEntity->GetSchematycObject())
					{
						pObject->ProcessSignal(SOnFire(), GetGUID());
					}

					m_bIsInteracted = false;
				}
			}
		}
		else
		{
			m_steeringControlValue = Lerp(m_steeringControlValue, 0.0f, gEnv->pTimer->GetFrameTime() * 2.5f);
			SetSteeringAngle(CryTransform::CAngle::FromDegrees(m_steeringControlValue * 0.5f));

			m_fireControlValue = Lerp(m_fireControlValue, 0.0f, gEnv->pTimer->GetFrameTime() * 5.0f);
		}


		{
			QuatT rel = pSkeleton->GetAbsJointByID(powerId);

			m_pCachedCharacter->GetISkeletonAnim()->PushPoseModifier(3, m_posePowerControlModifier, "Power Control");

			rel.t = rel.t;
			rel.q = rel.q + Quat::CreateRotationX(DEG2RAD(m_powerControlValue));

			m_posePowerControlModifier->PushOrientation(powerId, IAnimationOperatorQueue::eOp_Override, rel.q);
		}

		{
			QuatT rel = pSkeleton->GetAbsJointByID(steeringId);

			m_pCachedCharacter->GetISkeletonAnim()->PushPoseModifier(3, m_poseSteeringControlModifier, "Steering Control");

			rel.t = rel.t;
			rel.q = rel.q + Quat::CreateRotationY(DEG2RAD(m_steeringControlValue));

			m_poseSteeringControlModifier->PushOrientation(steeringId, IAnimationOperatorQueue::eOp_Override, rel.q);
		}

		{
			QuatT rel = pSkeleton->GetAbsJointByID(fireId);

			m_pCachedCharacter->GetISkeletonAnim()->PushPoseModifier(3, m_poseFireControlModifier, "Fire Control");

			rel.t = rel.t;
			rel.q = rel.q + Quat::CreateRotationX(DEG2RAD(m_fireControlValue));

			m_poseFireControlModifier->PushOrientation(fireId, IAnimationOperatorQueue::eOp_Override, rel.q);
		}

	}

}