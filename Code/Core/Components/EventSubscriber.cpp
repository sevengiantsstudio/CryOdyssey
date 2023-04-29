#include "StdAfx.h"
#include "EventSubscriber.h"

#include <Core/GameEvents/GameEvents.h>

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>

static void RegisterGameEventsComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope entityScope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = entityScope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CGameEventsComponent));

		// Functions
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGameEventsComponent::SendSignalSpawnPlayer, "{D8BE4DE7-1C6F-4A6B-BBF0-FBE428BA72BA}"_cry_guid, "Send SpawnPlayer");
			pFunction->SetDescription("Sends the SpawnPlayer event to all subscribed entities, or the specific entity if specified.");
			pFunction->BindInput(1, 'ent', "Entity", "The entity to send the game event too. If not set, will send the signal to all entity that are subscribed.");
			entityScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGameEventsComponent::SendSignalPlayerStateMovement, "{AA1EF343-904C-4B70-B3E7-841D602F89BC}"_cry_guid, "Send PlayerStateMovement");
			pFunction->BindInput(1, 'ent', "Entity", "The entity to send the game event too. If not set, will send the signal to all entity that are subscribed.");
			entityScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGameEventsComponent::SendSignalPlayerStateFly, "{D67748ED-66FF-4EAA-AC1A-B1692B71898E}"_cry_guid, "Send PlayerStateFly");
			pFunction->BindInput(1, 'ent', "Entity", "The entity to send the game event too. If not set, will send the signal to all entity that are subscribed.");
			entityScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGameEventsComponent::SendSignalPlayerStateSwim, "{E2764671-BCD1-4B91-B042-F5C831340036}"_cry_guid, "Send PlayerStateSwim");
			pFunction->BindInput(1, 'ent', "Entity", "The entity to send the game event too. If not set, will send the signal to all entity that are subscribed.");
			entityScope.Register(pFunction);
		}

		// Signals
		{
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SSpawnPlayerSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerStateMovement));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerStateFly));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerStateSwim));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerStartInteraction));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerStopInteraction));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerEnterSelectEntity));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(SPlayerLeaveSelectEntity));
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterGameEventsComponent)

CGameEventsComponent::~CGameEventsComponent()
{
	CGameEvents::GetInstance()->UnsubscribeFromEvents(this, GetGameEventsMask());
}

// IEntityComponent
void CGameEventsComponent::Initialize()
{
	CGameEvents::GetInstance()->SubscribeToEvents(this, GetGameEventsMask());
}
// ~IEntityComponent

// IGameEventListener
IEntity* CGameEventsComponent::GetEntity() const
{
	return m_pEntity;
}

void CGameEventsComponent::OnEvent(const SGameEventSignalBase& event)
{
	if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
	{
		switch (event.m_type)
		{

		case SSpawnPlayerSignal::type:
			pSchematycObject->ProcessSignal(static_cast<const SSpawnPlayerSignal&>(event), GetGUID());
			break;

		case SPlayerStateMovement::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerStateMovement&>(event), GetGUID());
			break;

		case SPlayerStateFly::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerStateFly&>(event), GetGUID());
			break;

		case SPlayerStateSwim::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerStateSwim&>(event), GetGUID());
			break;

		case SPlayerStartInteraction::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerStartInteraction&>(event), GetGUID());
			break;

		case SPlayerStopInteraction::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerStopInteraction&>(event), GetGUID());
			break;

		case SPlayerEnterSelectEntity::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerEnterSelectEntity&>(event), GetGUID());
			break;

		case SPlayerLeaveSelectEntity::type:
			pSchematycObject->ProcessSignal(static_cast<const SPlayerLeaveSelectEntity&>(event), GetGUID());
			break;
		}
	}
}
// ~IGameEventListener

void CGameEventsComponent::SendSignalSpawnPlayer(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SSpawnPlayerSignal());
}

void CGameEventsComponent::SendSignalPlayerStateMovement(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SPlayerStateMovement());
}

void CGameEventsComponent::SendSignalPlayerStateFly(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SPlayerStateFly());
}

void CGameEventsComponent::SendSignalPlayerStateSwim(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SPlayerStateSwim());
}

void CGameEventsComponent::SendSignalPlayerStartInteraction(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SPlayerStartInteraction());
}

void CGameEventsComponent::SendSignalPlayerStopInteraction(const Schematyc::ExplicitEntityId& entityId) const
{
	CGameEvents::GetInstance()->SendEvent(gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)), SPlayerStopInteraction());
}


EGameEventMask CGameEventsComponent::GetGameEventsMask() const
{
	EGameEventMask eventMask = EGameEvent::None;
	for (int i = 0; i < m_events.Size(); i++)
		eventMask.Add(m_events.At(i));

	return eventMask;
}