#include "StdAfx.h"
#include "Note.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterNoteComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CNoteComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterNoteComponent)

void CNoteComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CNoteComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::GameplayStarted;
}

void CNoteComponent::ProcessEvent(const SEntityEvent& event)
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

void CNoteComponent::StartUseAction()
{
}

bool CNoteComponent::StopUseAction()
{
	return true;
}

bool CNoteComponent::IsCanUseAction() const
{
	return true;
}
