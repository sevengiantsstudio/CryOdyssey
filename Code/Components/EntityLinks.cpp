// Copyright 2019-2022 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "EntityLinks.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CryEntitySystem/IEntitySystem.h>

static void RegisterEntityLinksComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEntityLinksComponent));
		// Functions
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::LinkToEntity, "{A245A88E-5B4D-4C21-9D7C-8217CA81E3F9}"_cry_guid, "LinkToEntity");
			pFunction->SetDescription("Links to the given entity.");
			pFunction->BindOutput(0, 'res', "Succcess", "True if the link was created successfully. False otherwise.");
			pFunction->BindInput(1, 'ent', "Entity", "The Entity to link to.");
			pFunction->BindInput(2, 'nam', "Link Name", "The name given to the new link.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::GetEntityByLink, "{2F70D9F3-3066-4B4C-89F1-0BB155AAA281}"_cry_guid, "GetLinkedEntity");
			pFunction->SetDescription("Returns the entity associated with a link by name.");
			pFunction->BindOutput(0, 'res', "Succcess", "True if the link was found. False otherwise.");
			pFunction->BindInput(1, 'lnam', "Link Name", "The name of the link used to get the entity.");
			pFunction->BindOutput(2, 'lent', "Entity", "The entity associated with the link.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::RenameEntityLink, "{A5644EE2-1F2C-491A-A196-AF711BB180B9}"_cry_guid, "RenameLink");
			pFunction->SetDescription("Renames the entity link to the new name.");
			pFunction->BindOutput(0, 'res', "Succcess", "True if the link was renamed successfully. False otherwise.");
			pFunction->BindInput(1, 'oldn', "Current Name", "The name of the link to rename from.");
			pFunction->BindInput(2, 'newn', "New Name", "The new name to give the link.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::UnlinkByName, "{A0866CF1-8EB8-496A-A56D-DDFCDCC83980}"_cry_guid, "UnlinkByName");
			pFunction->SetDescription("Removes the entity link by name.");
			pFunction->BindInput(1, 'nam', "Link Name", "The name of the link to remove.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::UnlinkByEntity, "{5EA7F596-71E7-4D03-960C-801469F2CB2B}"_cry_guid, "UnlinkByEntity");
			pFunction->SetDescription("Removes all entity links connected to specified entity.");
			pFunction->BindInput(1, 'ent', "Entity", "The entity to remove the links from.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CEntityLinksComponent::ClearAllLinks, "{0054136A-069C-41DE-9EEF-A444B056D8AD}"_cry_guid, "ClearAllLinks");
			pFunction->SetDescription("Removes all entity links on the current entity.");
			componentScope.Register(pFunction);
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterEntityLinksComponent)

bool CEntityLinksComponent::GetEntityByLink(const Schematyc::CSharedString & name, Schematyc::ExplicitEntityId& entityIdOut) const
{
	if (IEntityLink* pLink = GetEntityLinkByName(name))
	{
		entityIdOut = static_cast<Schematyc::ExplicitEntityId>(pLink->entityId);
		return true;
	}

	entityIdOut = Schematyc::ExplicitEntityId::Invalid;
	return false;
}

bool CEntityLinksComponent::LinkToEntity(const Schematyc::ExplicitEntityId & entityId, const Schematyc::CSharedString & name) const
{
	if (IEntity* pTargetEntity = gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)))
	{
		if (m_pEntity->AddEntityLink(name.c_str(), static_cast<EntityId>(entityId), pTargetEntity->GetGuid()) != nullptr)
		{
			return true;
		}
	}

	return false;
}

bool CEntityLinksComponent::RenameEntityLink(const Schematyc::CSharedString & oldName, const Schematyc::CSharedString & newName) const
{
	if (IEntityLink* pLink = GetEntityLinkByName(oldName))
	{
		m_pEntity->RenameEntityLink(pLink, newName.c_str());
		return true;
	}

	return false;
}

void CEntityLinksComponent::UnlinkByName(const Schematyc::CSharedString & name) const
{
	if (IEntityLink* pLink = GetEntityLinkByName(name))
	{
		m_pEntity->RemoveEntityLink(pLink);
	}
}

void CEntityLinksComponent::UnlinkByEntity(const Schematyc::ExplicitEntityId & entityId) const
{
	IEntityLink* pLink = m_pEntity->GetEntityLinks();
	while (pLink)
	{
		if (pLink->entityId == static_cast<EntityId>(entityId))
		{
			m_pEntity->RemoveEntityLink(pLink);
		}

		pLink = pLink->next;
	}
}

IEntityLink* CEntityLinksComponent::GetEntityLinkByName(const Schematyc::CSharedString& name) const 
{
	IEntityLink* pLink = m_pEntity->GetEntityLinks();
	while (pLink)
	{
		if (stricmp(pLink->name, name.c_str()) == 0)
		{
			return pLink;
		}
		pLink = pLink->next;
	}

	return nullptr;
}

void CEntityLinksComponent::ClearAllLinks() const
{
	m_pEntity->RemoveAllEntityLinks();
}
