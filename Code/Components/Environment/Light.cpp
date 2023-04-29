#include "StdAfx.h"
#include "Light.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include "Components/InterfaceComponent.h"

static void RegisterInteractiveEntityComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CLightComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInteractiveEntityComponent)

void CLightComponent::Initialize()
{
	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
}

Cry::Entity::EventFlags CLightComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::GameplayStarted;
}

void CLightComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::GameplayStarted:
	{
		m_pInterfaceComponent->AddInterface<IInteractAction>(this);
		m_pPointLightComponent = m_pEntity->GetComponent<Cry::DefaultComponents::CPointLightComponent>();
		m_pProjectorLightComponent = m_pEntity->GetComponent<Cry::DefaultComponents::CProjectorLightComponent>();
		if (m_pPointLightComponent)
		{
			m_bActive = m_pPointLightComponent->IsEnabled();
		}
		if (m_pProjectorLightComponent)
		{
			m_bActive = m_pProjectorLightComponent->IsEnabled();
		}
	}
	break;
	}
}

void CLightComponent::StartUseAction()
{
	if (m_pPointLightComponent)
	{
		m_bActive = !m_bActive;
		m_pPointLightComponent->Enable(m_bActive);
		return;
	}
	if (m_pProjectorLightComponent)
	{
		m_bActive = !m_bActive;
		m_pProjectorLightComponent->Enable(m_bActive);
		return;
	}
}

bool CLightComponent::StopUseAction()
{
	return true;
}

bool CLightComponent::IsCanUseAction() const
{
	return true;
}
