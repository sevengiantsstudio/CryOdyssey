// Copyright 2019-2022 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>

class CEntityLinksComponent final : public IEntityComponent
{
public:
	static void ReflectType(Schematyc::CTypeDesc<CEntityLinksComponent>& desc)
	{
		desc.SetLabel("Entity Links");
		desc.SetEditorCategory("Misc");
		desc.SetDescription("Allows creating, removing and naming links. Provides ability to get the entity connected to a named link.");
		desc.SetGUID("{5F8541DB-6B3C-4F8B-95E5-D94F59E4042A}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });
	}

	CEntityLinksComponent() = default;
	virtual ~CEntityLinksComponent() override final = default;

	bool GetEntityByLink(const Schematyc::CSharedString& name, Schematyc::ExplicitEntityId& entityIdOut) const;
	bool LinkToEntity(const Schematyc::ExplicitEntityId& entityId, const Schematyc::CSharedString & name) const;
	bool RenameEntityLink(const Schematyc::CSharedString & oldName, const Schematyc::CSharedString & newName) const;
	void UnlinkByName(const Schematyc::CSharedString& name) const;
	void UnlinkByEntity(const Schematyc::ExplicitEntityId& entityId) const;
	void ClearAllLinks() const;

private:
	IEntityLink* GetEntityLinkByName(const Schematyc::CSharedString& name) const;
};