//Light Deer Games | 2021-2022 | Skyscraper project

#include "StdAfx.h"
#include "ExplosionComponent.h"

#include <CryPhysics/physinterface.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>

static void RegisterExplosionComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CExplosionComponent));
		// Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CExplosionComponent::Simulate, "{9873A84C-D3EA-420F-A362-436CD1E5A7E1}"_cry_guid, "Simulate");
			componentScope.Register(pFunction);
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterExplosionComponent)

void CExplosionComponent::Initialize()
{

}

Cry::Entity::EventFlags CExplosionComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Initialize | Cry::Entity::EEvent::Reset;
}

void CExplosionComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Initialize:
	{

	}
	break;
	case Cry::Entity::EEvent::Reset:
	{
	}
	break;
	}
}

void CExplosionComponent::Simulate()
{
	pe_explosion explosion;
	explosion.epicenter = explosion.epicenterImp = GetWorldTransformMatrix().GetTranslation();
	explosion.rmin = 0.2f;
	explosion.rmax = explosion.r = m_radius;
	explosion.forceDeformEntities = true;
	explosion.impulsivePressureAtR = m_impulse;
	explosion.explDir = m_direction;
	
	gEnv->pPhysicalWorld->SimulateExplosion(&explosion);
}
