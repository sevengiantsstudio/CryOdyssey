#include "StdAfx.h"
#include "ItemComponent.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>

static void RegisterItemComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CItemComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterItemComponent)

void CItemComponent::Initialize()
{
}

EntityEventMask CItemComponent::GetEventMask() const
{
	return EntityEventMask();
}

void CItemComponent::ProcessEvent(const SEntityEvent& event)
{
}
