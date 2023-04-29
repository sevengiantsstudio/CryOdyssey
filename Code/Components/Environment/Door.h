#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include "Interfaces/IDoor.h"

class CInterfaceComponent;

class CDoorComponent final : public IEntityComponent, public IDoor
{
public:
	CDoorComponent() = default;
	virtual ~CDoorComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CDoorComponent>& desc)
	{
		desc.SetLabel("Door");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{E99E2D9A-44BE-436B-82B4-B5E714CE3F6D}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
	}

protected:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	CInterfaceComponent* m_pInterfaceComponent;
};