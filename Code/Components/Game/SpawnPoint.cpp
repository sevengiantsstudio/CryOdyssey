//Light Deer Games | 2021-2022 | Skyscraper project

#include "StdAfx.h"
#include "SpawnPoint.h"

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/StaticInstanceList.h>

static void RegisterSpawnPointComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSpawnPointComponent));
		// Functions
		{
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpawnPointComponent)

void CSpawnPointComponent::Initialize()
{
	if (gEnv->IsEditor())
	{
		GetEntity()->LoadGeometry(0, "%Editor%/Objects/spawnpointhelper.cgf");
	}
}

IEntityComponent::ComponentEventPriority CSpawnPointComponent::GetEventPriority() const
{
	return 16;
}

Cry::Entity::EventFlags CSpawnPointComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Reset;
}

void CSpawnPointComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Reset:
	{
		if (gEnv->IsEditor())
		{
			if (event.nParam[0] == 1)
			{
				RemoveSpawnPoint();
			}
		}
	}
	break;
	}
}
