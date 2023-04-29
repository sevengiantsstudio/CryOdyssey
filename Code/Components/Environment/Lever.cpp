#include "StdAfx.h"
#include "Lever.h"

#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterLeverComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CLeverComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterLeverComponent)

void CLeverComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CLeverComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::GameplayStarted;
}

void CLeverComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	{
		m_pInterfaceComponent->AddInterface<ILever>(this);
	}
	break;
	}
}