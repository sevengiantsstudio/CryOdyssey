#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>
#include "Interfaces/IInteractAction.h"

class CInterfaceComponent;

class CSwitchComponent final : public IEntityComponent, public IInteractAction
{
public:
	CSwitchComponent() = default;
	virtual ~CSwitchComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent>& desc)
	{
		desc.SetLabel("Switch");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{3E9F650D-3BD0-41B5-BD45-ADE334CD21C5}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
	}

protected:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	//IInteractAction
	virtual void StartUseAction() override;

	virtual bool StopUseAction() override;

	virtual bool IsCanUseAction() const override;
	//~IInteractAction

private:
	CInterfaceComponent* m_pInterfaceComponent;
};