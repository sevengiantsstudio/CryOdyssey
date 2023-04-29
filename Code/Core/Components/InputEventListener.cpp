#include "StdAfx.h"
#include "InputEventListener.h"

#include <Core/GameInput/InputEvents.h>

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>

static void RegisterInputEventsComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope entityScope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = entityScope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CInputEventsComponent));

		// Functions
		{
		}

		// Signals
		{
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SInteractInputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SRunningInputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SCrouchInputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SFire1InputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SFire2InputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SJumpInputSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SFlashlightInputSignal));
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInputEventsComponent)

CInputEventsComponent::~CInputEventsComponent()
{
	CInputEvents::GetInstance()->UnsubscribeFromEvents(this, GetInputEventsMask());
}

// IEntityComponent
void CInputEventsComponent::Initialize()
{
	CInputEvents::GetInstance()->SubscribeToEvents(this, GetInputEventsMask());
}
// ~IEntityComponent

// IInputEventListener
void CInputEventsComponent::OnEvent(const SInputEventSignalBase& event)
{
	if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
	{
		switch (event.m_type)
		{
		case SInteractInputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SInteractInputSignal&>(event), GetGUID());
			break;
		case SRunningInputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SRunningInputSignal&>(event), GetGUID());
			break;
		case SCrouchInputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SCrouchInputSignal&>(event), GetGUID());
			break;
		case SFire1InputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SFire1InputSignal&>(event), GetGUID());
			break;
		case SFire2InputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SFire2InputSignal&>(event), GetGUID());
			break;
		case SJumpInputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SJumpInputSignal&>(event), GetGUID());
			break;
		case SFlashlightInputSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SFlashlightInputSignal&>(event), GetGUID());
			break;
		}
	}
}
// ~IInputEventListener

EInputEventMask CInputEventsComponent::GetInputEventsMask() const
{
	EInputEventMask eventMask = EInputEvent::None;
	for (int i = 0; i < m_events.Size(); i++)
		eventMask.Add(m_events.At(i));

	return eventMask;
}