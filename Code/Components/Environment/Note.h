#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>
#include "Interfaces/IInteractAction.h"

class CInterfaceComponent;

class CNoteComponent final : public IEntityComponent, public IInteractAction
{
public:
	CNoteComponent() = default;
	virtual ~CNoteComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CNoteComponent>& desc)
	{
		desc.SetLabel("Note");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{038761A3-0A2A-4DE1-8204-F5EFB2A56F76}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
		desc.AddMember(&CNoteComponent::m_noteUniqueName, 'text', "UniqueName", "Unique Name", "", "");
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

	Schematyc::CSharedString m_noteUniqueName;
};