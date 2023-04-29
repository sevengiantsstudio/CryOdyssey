#include "StdAfx.h"
#include "AreaTrigger.h"

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

#include "FlowNodes/EntityFlowNode.h"

class CAreaTriggerNodeRegistrator
{
public:
	~CAreaTriggerNodeRegistrator()
	{
		if (pFlowNodeFactory)
			pFlowNodeFactory->UnregisterFactory();
		pFlowNodeFactory = nullptr;
	}

	void Register()
	{
		pFlowNodeFactory = new CEntityFlowNodeFactory("Game:AreaTrigger");

		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Enable"));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Disable"));

		pFlowNodeFactory->m_activateCallback = CAreaTriggerComponent::OnTriggerFlownodeActivation;

		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("Enter"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("Leave"));

		pFlowNodeFactory->Close();
	}

protected:
	_smart_ptr<CEntityFlowNodeFactory> pFlowNodeFactory = nullptr;
};

CAreaTriggerNodeRegistrator g_areaTriggerEntityRegistrator;

static void RegisterTriggerComponent(Schematyc::IEnvRegistrar& registrar)
{
	g_areaTriggerEntityRegistrator.Register();

	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CAreaTriggerComponent));
		// Functions
		{
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterTriggerComponent)


void CAreaTriggerComponent::OnTriggerFlownodeActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const CEntityFlowNode* pNode)
{
	auto* pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	auto* pTriggerEntity = pEntity->GetComponent<CAreaTriggerComponent>();

	if (IsPortActive(pActInfo, eInputPorts_Enable))
	{
		pTriggerEntity->m_bActive = GetPortBool(pActInfo, eInputPorts_Enable);
	}
	else if (IsPortActive(pActInfo, eInputPorts_Disable))
	{
		pTriggerEntity->m_bActive = false;
	}
}

void CAreaTriggerComponent::ActivateFlowNodeOutput(const int portIndex, const TFlowInputData& inputData)
{
	SEntityEvent evnt;
	evnt.event = ENTITY_EVENT_ACTIVATE_FLOW_NODE_OUTPUT;
	evnt.nParam[0] = portIndex;

	evnt.nParam[1] = (INT_PTR)&inputData;
	CAreaTriggerComponent::GetEntity()->SendEvent(evnt);
}

void CAreaTriggerComponent::Initialize()
{
	CreateHelper();
}

EntityEventMask CAreaTriggerComponent::GetEventMask() const
{
	return EEntityEvent::GameplayStarted | 
		EEntityEvent::Reset | 
		EEntityEvent::EntityEnteredThisArea | 
		EEntityEvent::EntityLeftThisArea;
}

void CAreaTriggerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case EEntityEvent::EntityEnteredThisArea:
	{
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
		if (pEntity->GetFlags() & ENTITY_FLAG_LOCAL_PLAYER && m_bActive)
		{
			CryLogAlways("Entity %s entered the area", gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0])->GetName());
			ActivateFlowNodeOutput(eOutputPorts_Enter, TFlowInputData((EntityId)event.nParam[0]));
		}
	}
	break;
	case EEntityEvent::EntityLeftThisArea:
	{
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
		if (pEntity->GetFlags() & ENTITY_FLAG_LOCAL_PLAYER && m_bActive)
		{
			CryLogAlways("Entity %s left the area", gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0])->GetName());
			ActivateFlowNodeOutput(eOutputPorts_Leave, TFlowInputData((EntityId)event.nParam[0]));
		}
	}
	break;
	case EEntityEvent::GameplayStarted:
	{
		RemoveHelper();
	}
	break;
	case EEntityEvent::Reset:
	{
		CreateHelper();
	}
	break;
	}
}

void CAreaTriggerComponent::CreateHelper()
{
	GetEntity()->LoadGeometry(0, "%Editor%/Objects/T.cgf");
}

void CAreaTriggerComponent::RemoveHelper()
{
	GetEntity()->ClearSlots();
}
