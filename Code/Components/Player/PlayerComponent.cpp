#include "StdAfx.h"
#include "PlayerComponent.h"

#include "GamePlugin.h"

#include <CryRenderer/IRenderAuxGeom.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>
#include <CryCore/StaticInstanceList.h>

#include <CryAISystem/IAISystem.h>
#include <CryAISystem/IAuditionMap.h>

#include <CryPhysics\physinterface.h>
#include <CryPhysics/IPhysics.h>

#include <CryInput/IHardwareMouse.h>

#include <CryGame/GameUtils.h>
#include <CryMath/Random.h>

#include <Core/GameInput/GameInput.h>

#include "Components/InterfaceComponent.h"
#include <Interfaces/ITakeDamage.h>

using namespace Game;


// REGISTER COMPONENT
static void RegisterPlayerCharacter(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		//Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::UpdateAnimation, "{6FDBE24A-1CB5-47BD-AA67-8B9AEA9600FB}"_cry_guid, "UpdateAnimation");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::UpdateStrafeMovement, "{415E498E-C714-4640-A3D9-D2673DC830CE}"_cry_guid, "UpdateStrafeMovement");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::EnableWeaponAim, "{61A40258-8AF9-4D4B-971A-04803C434D00}"_cry_guid, "EnableWeaponAim");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::DisableWeaponAim, "{6D186412-F43A-4AD9-82F3-75CEF8D7162D}"_cry_guid, "DisableWeaponAim");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::UpdateWeaponAim, "{5BB9C457-9F70-40EB-B5C5-4D4DF32B2B0E}"_cry_guid, "UpdateWeaponAim");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::UpdateSwimming, "{BE8E38B2-B896-4B1F-A929-D5530A23E032}"_cry_guid, "UpdateSwimming");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::SetFlyMode, "{89E06371-75F4-4181-ABCE-176C7387925C}"_cry_guid, "SetFlyMode");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::SetMovementMode, "{78CF60C3-CE2C-46E6-A7EA-83FEC8F18F92}"_cry_guid, "SetMovementMode");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::UpdateFlying, "{11EF6A27-3D74-49FF-BD21-30B4F194D358}"_cry_guid, "UpdateFlying");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::StartSprint, "{6A4BB350-7736-4FC8-B325-094627D255D8}"_cry_guid, "StartSprint");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::StopSprint, "{05CE5C9E-0ECE-4388-9365-D7E3C66A95F5}"_cry_guid, "StopSprint");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::GetAllowInput, "{342864E5-C156-4ABA-9BA7-21FCC495EF81}"_cry_guid, "GetAllowInput");
			pFunction->BindOutput(0, 'alw', "Value", "...");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::Jump, "{C33391DE-3B5C-4A5E-820C-33E8450E5955}"_cry_guid, "Jump");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::GetMovementRequest, "{A076A92A-D4A9-4BF9-823A-274CC8E66B8B}"_cry_guid, "GetMovementRequest");
			pFunction->BindOutput(0, 'rqs', "Movement Request", "...");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::StartInteraction, "{F4ACE5E3-A97C-4B4A-BDFE-68ABE7A90563}"_cry_guid, "StartInteraction");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::StopInteraction, "{667456C9-8CAF-468D-9B47-C415952535D6}"_cry_guid, "StopInteraction");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::EnableInput, "{463BF8A8-469D-4343-89ED-966B1CB239C1}"_cry_guid, "EnableInput");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerComponent::DisableInput, "{93F7996F-1C65-4E15-9285-275EA1A7DAD3}"_cry_guid, "DisableInput");
			componentScope.Register(pFunction);
		}

		// Signals
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnSwimStateEnter);
			componentScope.Register(pSignal);
		}
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnSwimStateExit);
			componentScope.Register(pSignal);
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerCharacter);

CPlayerComponent::~CPlayerComponent()
{
	DisableInput();
}

void CPlayerComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();

	m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
	m_pCharacterController->SetTransformMatrix(Matrix34::Create(Vec3(1.f), IDENTITY, Vec3(0, 0, 1.f)));
	m_pCharacterController->Physicalize();

	m_pAnimationController = m_pEntity->GetOrCreateComponent<CPlayerAnimationComponent>();

	m_pInteractionController = m_pEntity->GetOrCreateComponent<CPlayerInteractionComponent>();

	m_currentSpeed = m_walkSpeed;
	m_smoothSpeed = m_currentSpeed;

	m_pInterfaceComponent->AddInterface<IPawn>(this);
}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Initialize | Cry::Entity::EEvent::Update |
		Cry::Entity::EEvent::Reset | Cry::Entity::EEvent::PrePhysicsUpdate |
		Cry::Entity::EEvent::PhysicsCollision | Cry::Entity::EEvent::GameplayStarted |
		Cry::Entity::EEvent::AnimationEvent | Cry::Entity::EEvent::PhysicsThreadPostStep;
		
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Initialize:
	{
		pe_params_pos pp;
		pp.doubleBufCoords = true;
		m_pEntity->GetPhysics()->SetParams(&pp);
	}
	break;
	case Cry::Entity::EEvent::GameplayStarted:
	case Cry::Entity::EEvent::Reset:
	{
		if (gEnv->IsEditor())
		{
			if (event.nParam[0] == 1)
			{

			}
			if (event.nParam[0] == 0)
			{
			}
		}
		else if (!gEnv->IsEditor())
		{

		}
	}
	break;

	case Cry::Entity::EEvent::AnimationEvent:
	{
	}
	break;

	case Cry::Entity::EEvent::Update:
	{	
		m_pInteractionController->OnUpdate();
	}
	break;

	case Cry::Entity::EEvent::PhysicsThreadPostStep:
	{
	}
	break;

	case Cry::Entity::EEvent::PrePhysicsUpdate:
	{
	}
	break;

	case Cry::Entity::EEvent::PhysicsCollision:
	{
	}
	break;

	}
}


void CPlayerComponent::SpawnFlashlight()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(string().Format("schematyc::entities::ent_flashlight"));
	spawnParams.sName = "FLASHLIGHT";
	spawnParams.vPosition = ZERO;

	if (IEntity* pFlashlightEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		//m_flashlightEntityId = pFlashlightEntity->GetId();

		//if (m_pAnimationComponent)
		//{
		//	if (ICharacterInstance* pCharacter = m_pAnimationComponent->GetCharacter())
		//	{
		//		if (IAttachment* pAttachment = pCharacter->GetIAttachmentManager()->GetInterfaceByName("socket_flashlight"))
		//		{
		//			CEntityAttachment* pFlashlghtAttachmentObject = new CEntityAttachment();
		//			pFlashlghtAttachmentObject->SetEntityId(pFlashlightEntity->GetId());
		//			pAttachment->AddBinding(pFlashlghtAttachmentObject);
		//		}
		//	}
		//}
	}
}

void CPlayerComponent::EnableInput()
{
	m_bAllowInput = true;
}

void CPlayerComponent::DisableInput()
{
	m_bAllowInput = false;
}

void CPlayerComponent::ActivateView()
{
}

void CPlayerComponent::UpdateStrafeMovement()
{
	if (m_pCharacterController && m_bAllowInput)
	{
		const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();
		Vec3 movementRequest = CGameInput::GetInstance()->GetMovementRequest();

		if (movementRequest.len() == 0.0f)
		{
			movementRequest = m_pEntity->GetForwardDir();
		}
		else
		{
			m_lastMovementRequest = movementRequest;
		}

		if (ICharacterInstance* character = m_pAnimationController->GetPlayerCharacter())
		{
			ISkeletonAnim& skeletonAnim = *character->GetISkeletonAnim();

			QuatT relMove = skeletonAnim.GetRelMovement();
			Vec3 vel = skeletonAnim.GetCurrentVelocity();

			vel = relMove.q * m_lastMovementRequest * vel.len();

			m_pCharacterController->SetVelocity(vel);
		}

		if (CGameInput::GetInstance()->IsMovementRequested())
		{
			Quat rot = Quat::CreateRotationVDir(systemCamera.GetViewdir());
			rot.v.x = 0;
			rot.v.y = 0;

			Quat lastRot = m_pEntity->GetRotation();
			Quat qResult = Quat::CreateSlerp(lastRot, rot, gEnv->pTimer->GetFrameTime() * 25.f);

			GetEntity()->SetRotation(qResult);
		}

	}
}

void CPlayerComponent::UpdateSwimming()
{
	Vec3 movementRequest = CGameInput::GetInstance()->GetMovementRequest() * m_currentSpeed;

	if (movementRequest.len() == 0.0f)
	{
		movementRequest = m_pEntity->GetForwardDir() * 0.01f;
	}
	else
	{
		m_lastMovementRequest = movementRequest;
	}

	Vec3 playerPos = m_pEntity->GetWorldPos();
	const float feetWaterLevel = gEnv->p3DEngine->GetWaterLevel(&playerPos);

	if (feetWaterLevel != WATER_LEVEL_UNKNOWN)
	{
		const float depth = feetWaterLevel - m_pEntity->GetWorldPos().z;

		if (depth >= 0.0f)
		{
			if (depth <= 0.75f)
			{
				if (Schematyc::IObject* pObject = m_pEntity->GetSchematycObject())
				{
					pObject->ProcessSignal(SOnSwimStateExit(), GetGUID());
				}
			}

			if (gEnv->p3DEngine)
			{
				const float relativeSpeed = max(0.2f, m_pCharacterController->GetVelocity().GetLength() * 0.5f);
				gEnv->p3DEngine->AddWaterRipple(GetEntity()->GetWorldPos(), 1.0f, relativeSpeed * 2);
			}
		}

		float dist = depth;
		float minDistance = 0.75f;
		float maxDistance = 0.85f;
		float fractionalPosition = (maxDistance - crymath::clamp(dist, 0.0f, 1.0f)) / (maxDistance - minDistance);
		float force = fractionalPosition;
		movementRequest.z = -force * 1.5f;
	}


	if (m_pCharacterController && m_bAllowInput)
	{
		float speedMul = 1.5f;

		movementRequest.x *= speedMul;
		movementRequest.y *= speedMul;


		if (ICharacterInstance* character = m_pAnimationController->GetPlayerCharacter())
		{
			ISkeletonAnim& skeletonAnim = *character->GetISkeletonAnim();

			QuatT relMove = skeletonAnim.GetRelMovement();
			Vec3 vel = skeletonAnim.GetCurrentVelocity();

			vel = relMove.q * vel;

			Quat rot = Quat::CreateRotationVDir(m_lastMovementRequest);

			m_pCharacterController->ChangeVelocity(rot * vel, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Jump);

		}
		movementRequest.x = 0.0f;
		movementRequest.y = 0.0f;
		m_pCharacterController->ChangeVelocity(movementRequest, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Add);

		Quat targetRot = Quat::CreateRotationVDir(m_lastMovementRequest);
		targetRot = Quat::CreateRotationZ(targetRot.GetRotZ());

		m_pEntity->SetRotation(Quat::CreateSlerp(m_pEntity->GetRotation(), targetRot, gEnv->pTimer->GetFrameTime() * 15.f));
	}
}

void CPlayerComponent::SetFlyMode()
{
	if (IPhysicalEntity* pPhysEnt = GetEntity()->GetPhysics())
	{
		pe_player_dynamics pd;
		
		pPhysEnt->GetParams(&pd);

		pd.bSwimming = true;
		pd.kAirControl = 0.0f;
		pd.kAirResistance = 0.0f;
		pd.gravity.zero();
		pPhysEnt->SetParams(&pd);
	}
}

void CPlayerComponent::SetMovementMode()
{
	if (IPhysicalEntity* pPhysEnt = GetEntity()->GetPhysics())
	{
		pe_player_dynamics pd;

		pPhysEnt->GetParams(&pd);

		pd.kInertia = 16.0f;
		pd.kInertiaAccel = 32.0f;
		pd.mass = 75.0f;
		pd.kAirControl = 0.5f;
		pd.kAirResistance = 0.0f;
		pd.nodSpeed = 2.5f;
		pd.minSlideAngle = 60.0f;
		pd.maxClimbAngle = 50.0f;
		pd.maxJumpAngle = 60.0f;
		pd.minFallAngle = 70.0f;
		pd.maxVelGround = 300.0f;
		pd.bSwimming = false;
		pd.bReleaseGroundColliderWhenNotActive = 0;
		pPhysEnt->SetParams(&pd);
	}
}

void CPlayerComponent::UpdateFlying()
{
	const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();
	Vec3 movementRequest = CGameInput::GetInstance()->GetMovementRequest() * m_currentSpeed;

	if (movementRequest.len() == 0.0f)
	{
		movementRequest = m_pEntity->GetForwardDir() * 0.01f;
	}
	else
	{
		m_lastMovementRequest = movementRequest;
	}

	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts[1];
	pSkipEnts[0] = m_pEntity->GetPhysics();

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(m_pEntity->GetWorldPos(),
		Vec3(0,0,-1000.0f),
		ent_static | ent_sleeping_rigid | ent_rigid | ent_independent | ent_terrain | ent_water,
		rwi_stop_at_pierceable | rwi_colltype_any,
		&rayhit,
		1, pSkipEnts, 2);

	Vec3 planeNormal = ZERO;

	if (hits)
	{
		float dist = rayhit.pt.GetDistance(m_pEntity->GetWorldPos());
		float minDistance = 0.35f;
		float maxDistance = 0.5f;
		float fractionalPosition = (maxDistance - crymath::clamp(dist, 0.0f, 1.0f)) / (maxDistance - minDistance);
		float force = fractionalPosition;
		movementRequest.z = force * 1.5f;	

		planeNormal = rayhit.n;
	}
	else
	{
		movementRequest.z = -3.5f;
	}

	if (m_pCharacterController && m_bAllowInput)
	{
		const Vec3  slideDirection = (m_pEntity->GetWorldRotation().GetColumn1() + m_pEntity->GetForwardDir()).GetNormalized();
		const float cosine = slideDirection.Dot(planeNormal);
		float groundSlopeDeg = RAD2DEG(acos_tpl(cosine)) - 90.0f;

		float speedMul = 1.5f;
		if (groundSlopeDeg > 7.0f)
		{
			speedMul = 1.25f;
		}
		if (groundSlopeDeg < -7.0f)
		{
			speedMul = 2.0f;
		}

		movementRequest.x *= speedMul;
		movementRequest.y *= speedMul;

		m_pCharacterController->ChangeVelocity(movementRequest, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Jump);

		Quat rot = Quat::CreateRotationVDir(systemCamera.GetViewdir());
		rot.v.x = 0;
		rot.v.y = 0;

		Quat lastRot = m_pEntity->GetRotation();
		Quat qResult = Quat::CreateSlerp(lastRot, rot, gEnv->pTimer->GetFrameTime() * 25.f);

		GetEntity()->SetRotation(qResult);
	}

}

void CPlayerComponent::UpdateAnimation()
{
	Interpolate(m_smoothSpeed, m_currentSpeed, m_accelerationTime, gEnv->pTimer->GetFrameTime());
	m_pAnimationController->SetMotionParameter(eMotionParamID_TravelSpeed, m_smoothSpeed);

	m_pAnimationController->SetPlayerMovement(CGameInput::GetInstance()->IsMovementRequested() && m_pCharacterController->IsOnGround());

	const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();

	Quat targetRot = Quat::CreateRotationVDir(Vec3(systemCamera.GetViewdir().x, systemCamera.GetViewdir().y, 0));
	m_pEntity->SetRotation(Quat::CreateSlerp(m_pEntity->GetRotation(), targetRot, gEnv->pTimer->GetFrameTime() * 15.0f));
}

void CPlayerComponent::UpdateWeaponAim()
{
	if (ICharacterInstance* pCharacterInstance = m_pAnimationController->GetPlayerCharacter())
	{
		ISkeletonPose* pSkeleton = pCharacterInstance->GetISkeletonPose();

		const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();


		IAnimationPoseBlenderDir* pPoseBlenderAim = pSkeleton->GetIPoseBlenderAim();
		if (pPoseBlenderAim)
		{
			Vec3 targetPos = ZERO;

			const auto rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
			ray_hit rayhit;
			static IPhysicalEntity* pSkipEnts[10];
			pSkipEnts[0] = GetEntity()->GetPhysics();

			const bool bRayHit = gEnv->pPhysicalWorld->RayWorldIntersection(systemCamera.GetPosition(), systemCamera.GetViewdir() * 1000.0f, ent_all, rayFlags, &rayhit, 1, GetEntity()->GetPhysicalEntity());

			if (bRayHit && rayhit.pCollider != nullptr)
			{
				targetPos = rayhit.pt;
			}
			else
			{
				Vec3 playerOffset = GetEntity()->GetWorldPos() + GetEntity()->GetForwardDir() * 10.0f;
				targetPos = systemCamera.GetPosition() + systemCamera.GetViewdir() * playerOffset.GetDistance(systemCamera.GetPosition());
			}

			pPoseBlenderAim->SetTarget(targetPos);

			//IPersistantDebug* pDebug = gEnv->pGameFramework->GetIPersistantDebug();
			//pDebug->Begin("CHPlayerDebug", false);
			//pDebug->AddSphere(targetPos, 1.0f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime() + 0.01f);
		}
	}
}

Vec3 CPlayerComponent::GetMovementRequest()
{
	return CGameInput::GetInstance()->GetMovementRequest();
}

const QuatT CPlayerComponent::GetHeadBoneLocation()
{
	QuatT headLoc;
	if (m_pAnimationController != nullptr)
	{
		headLoc = m_pAnimationController->GetHeadBoneLocation();
	}
	return headLoc;
}

void CPlayerComponent::EnableWeaponAim()
{
	if (ICharacterInstance* pCharacterInstance = m_pAnimationController->GetPlayerCharacter())
	{
		IAnimationPoseBlenderDir* pPoseBlenderAim = m_pAnimationController->GetPlayerCharacter()->GetISkeletonPose()->GetIPoseBlenderAim();
		if (pPoseBlenderAim)
		{
			pPoseBlenderAim->SetState(true);
		}
	}
}

void CPlayerComponent::DisableWeaponAim()
{
	if (ICharacterInstance* pCharacterInstance = m_pAnimationController->GetPlayerCharacter())
	{
		IAnimationPoseBlenderDir* pPoseBlenderAim = m_pAnimationController->GetPlayerCharacter()->GetISkeletonPose()->GetIPoseBlenderAim();
		if (pPoseBlenderAim)
		{
			pPoseBlenderAim->SetState(false);
		}
	}
}

void CPlayerComponent::StartSprint()
{
	m_currentSpeed = m_runSpeed;

	if (ICharacterInstance* pCharacter = m_pAnimationController->GetPlayerCharacter())
	{
		if (IProxy* pWeaponProxy = pCharacter->GetIAttachmentManager()->GetProxyInterfaceByName("hit_test"))
		{
			pWeaponProxy->SetHideProxy(1);
		}
	}
}

void CPlayerComponent::StopSprint()
{
	m_currentSpeed = m_walkSpeed;
}

void CPlayerComponent::SetCrouch(bool val)
{
}

void CPlayerComponent::Jump()
{
	if (m_pCharacterController->IsOnGround())
	{
		m_pCharacterController->AddVelocity(Vec3(0, 0, m_jumpPower));
	}
}

void CPlayerComponent::StartInteraction()
{
	m_pInteractionController->OnEnter();
}

void CPlayerComponent::StopInteraction()
{
	m_pInteractionController->OnLeave();
}
