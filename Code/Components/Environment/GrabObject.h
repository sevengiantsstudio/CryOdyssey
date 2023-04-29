#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include "Interfaces/IGrabObject.h"

class CInterfaceComponent;

class CGrabObjectComponent final : public IEntityComponent, public IGrabObject
{
public:
	CGrabObjectComponent() = default;
	virtual ~CGrabObjectComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CGrabObjectComponent>& desc)
	{
		desc.SetLabel("GrabObject");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{4544E877-EB57-456C-9DB1-3B10291590A8}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
	}

protected:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	CInterfaceComponent* m_pInterfaceComponent;
};