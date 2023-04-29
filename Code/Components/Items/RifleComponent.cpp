#include "StdAfx.h"
#include "RifleComponent.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterRifleComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CRifleComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterRifleComponent)

void CRifleComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CRifleComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Initialize | EEntityEvent::Update;
}

void CRifleComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Initialize:
	{
		m_pInterfaceComponent->AddInterface<IItem>(this);
	}
	break;

	case EEntityEvent::Update:
	{
		if (m_bIsShooting)
		{
			m_currentFireTime -= 1.0f * gEnv->pTimer->GetFrameTime();
			if (m_currentFireTime <= 0.0f)
			{
				SpawnProjectile();
				m_currentFireTime = m_fireSpeed;
			}
		}
	} break;
	}
}

void CRifleComponent::StartUseAction()
{
	m_bIsShooting = true;
}

bool CRifleComponent::StopUseAction()
{
	m_bIsShooting = false;
	return true;
}

bool CRifleComponent::IsCanUseAction() const
{
	return true;
}

void CRifleComponent::SpawnProjectile()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(m_projectileClassname.value);
	spawnParams.sName = nullptr;
	spawnParams.vPosition = GetEntity()->GetWorldPos() + GetEntity()->GetForwardDir() * 0.7f + Vec3(0, 0, 0.1f); //test
	spawnParams.qRotation = GetEntity()->GetRotation();

	gEnv->pEntitySystem->SpawnEntity(spawnParams);
}
