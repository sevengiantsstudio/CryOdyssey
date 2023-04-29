#include "StdAfx.h"
#include "CinematicTrigger.h"
#include <CryEntitySystem/IEntity.h>

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>
#include <CryCore/StaticInstanceList.h>

#include <Core/Game.h>
#include "FlowNodes/EntityFlowNode.h"

//Flow node register
class CCinematicTriggerNodeRegistrator
{
public:
	~CCinematicTriggerNodeRegistrator()
	{
		if (pFlowNodeFactory)
			pFlowNodeFactory->UnregisterFactory();
		pFlowNodeFactory = nullptr;
	}

	void Register()
	{
		pFlowNodeFactory = new CEntityFlowNodeFactory("Game:CinematicTrigger");

		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Enable"));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Disable"));

		pFlowNodeFactory->m_activateCallback = CCinematicTrigger::OnTriggerFlownodeActivation;

		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("Enter"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("Leave"));

		pFlowNodeFactory->Close();
	}

protected:
	_smart_ptr<CEntityFlowNodeFactory> pFlowNodeFactory = nullptr;
};

CCinematicTriggerNodeRegistrator g_cinematicTriggerEntityRegistrator;
//~Flow node

static void RegisterCinematicTrigger(Schematyc::IEnvRegistrar& registrar)
{
	g_cinematicTriggerEntityRegistrator.Register();

	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCinematicTrigger));
		// Functions
		{

		}
		//Signals
		{
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SOnEnteredSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SOnLeaveSignal));
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterCinematicTrigger)

static void ReflectType(Schematyc::CTypeDesc<SOnEnteredSignal>& desc)
{
	desc.SetGUID("{858477C2-CC07-4A95-9814-965B05220188}"_cry_guid);
	desc.SetLabel("OnEntered");
	desc.AddMember(&SOnEnteredSignal::entityId, 'eid', "Entity", "Entity", "...", Schematyc::ExplicitEntityId::Invalid);
}

static void ReflectType(Schematyc::CTypeDesc<SOnLeaveSignal>& desc)
{
	desc.SetGUID("{E97CCD51-A455-4A32-864F-51177EB09DCD}"_cry_guid);
	desc.SetLabel("OnLeave");
	desc.AddMember(&SOnLeaveSignal::entityId, 'eid', "Entity", "Entity", "...", Schematyc::ExplicitEntityId::Invalid);
}

void CCinematicTrigger::OnTriggerFlownodeActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const CEntityFlowNode* pNode)
{
	auto* pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	auto* pTriggerEntity = pEntity->GetComponent<CCinematicTrigger>();

	if (IsPortActive(pActInfo, eInputPorts_Enable))
	{
		pTriggerEntity->m_bEnabled = GetPortBool(pActInfo, eInputPorts_Enable);
	}
	else if (IsPortActive(pActInfo, eInputPorts_Disable))
	{
		pTriggerEntity->m_bEnabled = false;
	}
}

void CCinematicTrigger::ActivateFlowNodeOutput(const int portIndex, const TFlowInputData& inputData)
{
	SEntityEvent evnt;
	evnt.event = ENTITY_EVENT_ACTIVATE_FLOW_NODE_OUTPUT;
	evnt.nParam[0] = portIndex;

	evnt.nParam[1] = (INT_PTR)&inputData;
	CCinematicTrigger::GetEntity()->SendEvent(evnt);
}

void CCinematicTrigger::Initialize()
{
	m_pTriggerComponent = m_pEntity->GetOrCreateComponent<IEntityTriggerComponent>();
	Setup();
}

void CCinematicTrigger::Enable(bool enable)
{
	m_bEnabled = enable;

	if (m_bEnabled == true)
	{
		SetTimer(timerId, static_cast<int>(m_fCheckTimer * 1000));
	}

	if (m_bEnabled == false)
	{
		KillTimer(timerId);
	}
}

void CCinematicTrigger::Setup()
{
	if (m_pTriggerComponent)
	{
		Vec3 vMin = -m_boxSize * 0.5f;
		Vec3 vMax = m_boxSize * 0.5f;
		m_boundingBox = AABB(vMin, vMax);
		m_pTriggerComponent->SetTriggerBounds(m_boundingBox);
	}	
}

void CCinematicTrigger::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	{
		if (m_bEnabled)
		{
			SetTimer(timerId, static_cast<int>(m_fCheckTimer * 1000));
		}
	}
	break;

	case Cry::Entity::EEvent::TimerExpired:
	{
		if (event.nParam[0] == timerId)
		{
			if (IsInRange() && IsVisible())
			{
				if (m_isVisible)
				{
					SetTimer(timerId, static_cast<int>(m_fCheckTimer * 1000));
					return;
				}

				SetTimer(timerDelayId, static_cast<int>(m_fDelay * 1000));

				m_isVisible = true;
			} 
			if (!IsInRange() || !IsVisible())
			{
				if (m_isVisible == false)
				{
					SetTimer(timerId, static_cast<int>(m_fCheckTimer * 1000));
					return;
				}

				if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
				{
					pSchematycObject->ProcessSignal(SOnLeaveSignal(), GetGUID());
				}

				ActivateFlowNodeOutput(eOutputPorts_Leave, TFlowInputData());

				m_isVisible = false;
				KillTimer(timerDelayId);
			}

			SetTimer(timerId, static_cast<int>(m_fCheckTimer * 1000));
		}
		if (event.nParam[0] == timerDelayId)
		{
			if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
			{
				pSchematycObject->ProcessSignal(SOnEnteredSignal(), GetGUID());
			}

			ActivateFlowNodeOutput(eOutputPorts_Enter, TFlowInputData());

			KillTimer(timerDelayId);
		}
	}
	break;
	case Cry::Entity::EEvent::EditorPropertyChanged:
	{
		Setup();
	}
	break;
	case Cry::Entity::EEvent::Reset:
	{
		KillTimer(timerId);
		KillTimer(timerDelayId);
		Setup();
	}
	break;
	}
}

EntityEventMask CCinematicTrigger::GetEventMask() const
{
	return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::TimerExpired |
		Cry::Entity::EEvent::EditorPropertyChanged | Cry::Entity::EEvent::Reset;
}

IGeometry* CCinematicTrigger::CreateGeometry() const
{
	primitives::box primitive;
	primitive.size = m_boxSize * 0.5f;
	primitive.Basis = IDENTITY;
	primitive.bOriented = 1;
	primitive.center = Vec3(0, 0, 0);

	return gEnv->pPhysicalWorld->GetGeomManager()->CreatePrimitive((int)primitive.type, &primitive);
}

bool CCinematicTrigger::IsVisible()
{
	Vec3 originPos = GetEntity()->GetWorldPos();
	Vec3 dBox = m_boxSize * 0.5f;

	Vec3 point0 = originPos + dBox;
	Vec3 point1 = Vec3(originPos.x + dBox.x, originPos.y + dBox.y, originPos.z - dBox.z);
	Vec3 point2 = Vec3(originPos.x + dBox.x, originPos.y - dBox.y, originPos.z + dBox.z);
	Vec3 point3 = Vec3(originPos.x + dBox.x, originPos.y - dBox.y, originPos.z - dBox.z);

	const CCamera& Camera = gEnv->pSystem->GetViewCamera();

	if (Camera.IsPointVisible(point0) && Camera.IsPointVisible(point1) &&
		Camera.IsPointVisible(point2) && Camera.IsPointVisible(point3))
	{
		point0 = Vec3(originPos.x - dBox.x, originPos.y + dBox.y, originPos.z + dBox.z);
		point1 = Vec3(originPos.x - dBox.x, originPos.y + dBox.y, originPos.z - dBox.z);
		point2 = Vec3(originPos.x - dBox.x, originPos.y - dBox.y, originPos.z + dBox.z);
		point3 = Vec3(originPos.x - dBox.x, originPos.y - dBox.y, originPos.z - dBox.z);

		if (Camera.IsPointVisible(point0) && Camera.IsPointVisible(point1) &&
			Camera.IsPointVisible(point2) && Camera.IsPointVisible(point3))
		{
			return true;
		}
	}
	return false;
}

bool CCinematicTrigger::IsInRange()
{
	if (auto pPlayer = Game::CGame::GetInstance()->GetPlayerEntity())
	{
		Vec3 originPos = GetEntity()->GetWorldPos();
		Vec3 playerPos = pPlayer->GetWorldPos();

		float dist = originPos.GetDistance(playerPos);

		if ((dist < m_fMinDistance) || (dist > m_fMaxDistance))
		{
			return false;
		}

		return true;
	}
	return false;
}

void CCinematicTrigger::Render(const IEntity& entity, const IEntityComponent& component, SEntityPreviewContext& context) const
{
	Matrix34 slotTransform = GetWorldTransformMatrix();

	IGeometry* pPrimGeom = CreateGeometry();
	geom_world_data geomWorldData;
	geomWorldData.R = Matrix33(slotTransform);
	geomWorldData.scale = slotTransform.GetUniformScale();
	geomWorldData.offset = slotTransform.GetTranslation();

	gEnv->pSystem->GetIPhysRenderer()->DrawGeometry(pPrimGeom, &geomWorldData, -1, 0, ZERO, context.debugDrawInfo.color);
	pPrimGeom->DrawWireframe(gEnv->pSystem->GetIPhysRenderer(), &geomWorldData, 1, -1);

	pPrimGeom->Release();
}