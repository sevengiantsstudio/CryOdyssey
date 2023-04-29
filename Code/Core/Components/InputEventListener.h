#pragma once

#include <Core/GameInput/IInputEvents.h>
#include <CryEntitySystem/IEntityComponent.h>

using namespace Game;

class CInputEventsComponent final : public IEntityComponent, public IInputEventGameListener
{
public:
	static void ReflectType(Schematyc::CTypeDesc<CInputEventsComponent>& desc)
	{
		desc.SetLabel("Input Events");
		desc.SetEditorCategory("Game");
		desc.SetDescription("The Input Events component allows sending and receiving input events.");
		desc.SetGUID("{AA901517-2DC6-465E-A7B7-5B4BD7BCC41D}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&CInputEventsComponent::m_events, 'evts', "Events", "Subscribed Events", "List of events that this component listens for", Schematyc::CArray<EInputEvent>());
	}

	CInputEventsComponent() = default;
	virtual ~CInputEventsComponent() override final;

protected:

	// IEntityComponent
	virtual void Initialize() override final;
	// ~IEntityComponent

	// IInputEventGameListener
	virtual void OnEvent(const SInputEventSignalBase& event) override final;
	// ~IInputEventGameListener

private:
	EInputEventMask GetInputEventsMask() const;

private:
	Schematyc::CArray<EInputEvent> m_events;
};