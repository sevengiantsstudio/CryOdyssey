#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include "Interfaces/ILever.h"

class CInterfaceComponent;

class CLeverComponent final : public IEntityComponent, public ILever
{
public:
	CLeverComponent() = default;
	virtual ~CLeverComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CLeverComponent>& desc)
	{
		desc.SetLabel("Lever");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{EE696B82-91B1-4110-8B84-447B21C313EB}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
	}

protected:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	CInterfaceComponent* m_pInterfaceComponent;
};