#include "StdAfx.h"
#include "Door.h"

#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterDoorComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CDoorComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterDoorComponent)

void CDoorComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CDoorComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::GameplayStarted;
}

void CDoorComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	{
		m_pInterfaceComponent->AddInterface<IDoor>(this);
	}
	break;
	}
}