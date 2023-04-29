#pragma once

#include <Core/GameEvents/IGameEvents.h>
#include <CryEntitySystem/IEntityComponent.h>

using namespace Game;

class CGameEventsComponent final : public IEntityComponent, public IGameEventListener
{
public:
	static void ReflectType(Schematyc::CTypeDesc<CGameEventsComponent>& desc)
	{
		desc.SetLabel("Game Events");
		desc.SetEditorCategory("Game");
		desc.SetDescription("The Game Events component allows sending and receiving game events.");
		desc.SetGUID("{29F15628-78B6-4AC9-918B-0207FD3BA52C}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&CGameEventsComponent::m_events, 'evts', "Events", "Subscribed Events", "List of events that this component listens for", Schematyc::CArray<EGameEvent>());
	}

	CGameEventsComponent() = default;
	virtual ~CGameEventsComponent() override final;

	// Signals
	void SendSignalSpawnPlayer(const Schematyc::ExplicitEntityId& entityId) const;
	void SendSignalPlayerStateMovement(const Schematyc::ExplicitEntityId& entityId) const;
	void SendSignalPlayerStateFly(const Schematyc::ExplicitEntityId& entityId) const;
	void SendSignalPlayerStateSwim(const Schematyc::ExplicitEntityId& entityId) const;
	void SendSignalPlayerStartInteraction(const Schematyc::ExplicitEntityId& entityId) const;
	void SendSignalPlayerStopInteraction(const Schematyc::ExplicitEntityId& entityId) const;

protected:

	// IEntityComponent
	virtual void Initialize() override final;
	// ~IEntityComponent

	// IGameEventListener
	virtual IEntity* GetEntity() const override final;
	virtual void OnEvent(const SGameEventSignalBase& event) override final;
	// ~IGameEventListener

private:
	EGameEventMask GetGameEventsMask() const;

private:
	Schematyc::CArray<EGameEvent> m_events;
};