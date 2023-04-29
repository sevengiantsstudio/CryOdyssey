#include "StdAfx.h"
#include "Switch.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterSwitchComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSwitchComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSwitchComponent)

void CSwitchComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CSwitchComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::GameplayStarted;
}

void CSwitchComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	{
		m_pInterfaceComponent->AddInterface<IInteractAction>(this);
	}
	break;
	}
}

void CSwitchComponent::StartUseAction()
{
	//Get camera entity
	IEntityLink* pLink = m_pEntity->GetEntityLinks();
	while (pLink)
	{
		if (IEntity* entity = gEnv->pEntitySystem->GetEntity(pLink->entityId))
		{
			if (CInterfaceComponent* pInterfaceComp = entity->GetComponent<CInterfaceComponent>())
			{
				if (IInteractAction* pInteractable = pInterfaceComp->GetInterface<IInteractAction>())
				{
					pInteractable->StartUseAction();
				}
			}
		}
		pLink = pLink->next;
	}
}

bool CSwitchComponent::StopUseAction()
{
	return true;
}

bool CSwitchComponent::IsCanUseAction() const
{
	return true;
}
