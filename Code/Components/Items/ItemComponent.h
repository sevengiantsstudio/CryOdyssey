#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <Interfaces/IItem.h>

class CItemComponent final : public IEntityComponent
{
public:
	CItemComponent() = default;
	virtual ~CItemComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CItemComponent>& desc)
	{
		desc.SetLabel("Item Component");
		desc.SetEditorCategory("Items");
		desc.SetDescription("Item Component.");
		desc.SetGUID("{8A4E58F1-C002-480D-9D0E-BBD071A56CA5}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&CItemComponent::m_name, 'nme', "Name", "Name", "...", "");
		desc.AddMember(&CItemComponent::m_socketName, 'stnm', "Socket", "Player socket", "...", "");
		desc.AddMember(&CItemComponent::m_ItemSpawnedClassName, 'clsn', "ItemSpawnedClassName", "Item Spawned Class Name", "Item Spawned Class Name", "");
		desc.AddMember(&CItemComponent::m_leadHand, 'ldhn', "LeadHand", "Lead hand", "...", "");
		desc.AddMember(&CItemComponent::m_bIsUsable, 'usbl', "IsUsable", "Is usable", "...", false);
	}

	Schematyc::EntityClassName GetItemSpawnedClassName() { return m_ItemSpawnedClassName; }
	Schematyc::CSharedString GetSocketName() { return m_socketName; }
	Schematyc::CSharedString GetItemName() { return m_name; }
	Schematyc::CSharedString GetLeadHand() { return m_leadHand; }
	bool GetItemUsable() { return m_bIsUsable; }

protected:
	// IEntityComponent
	virtual void Initialize() override final;
	virtual EntityEventMask GetEventMask() const override final;
	virtual void ProcessEvent(const SEntityEvent& event) override final;
	// ~IEntityComponent

private:
	Schematyc::CSharedString m_name;
	Schematyc::CSharedString m_socketName;
	Schematyc::EntityClassName m_ItemSpawnedClassName;
	Schematyc::CSharedString m_leadHand;
	bool m_bIsUsable;
};