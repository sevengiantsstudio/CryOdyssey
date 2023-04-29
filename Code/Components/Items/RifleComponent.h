#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/ResourceTypes.h>
#include "Interfaces/IItem.h"

class CInterfaceComponent;

class CRifleComponent final : public IEntityComponent, public IItem
{
public:
	CRifleComponent() = default;
	virtual ~CRifleComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CRifleComponent>& desc)
	{
		desc.SetLabel("Rifle");
		desc.SetEditorCategory("Items");
		desc.SetDescription("Rifle weapon component.");
		desc.SetGUID("{2C90412A-9F4B-49E0-ADE6-FBB3D5040315}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

		desc.AddMember(&CRifleComponent::m_fireSpeed, 'fspd', "FireSpeed", "Fire Speed", "Number of projectiles fired form this weapon per second", 1.0f);
		desc.AddMember(&CRifleComponent::m_projectileClassname, 'pcls', "ProjectileClass", "Projectile Class", "The entity class that represents this weapons projectile entity", "");
	}

protected:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	// IItem
	virtual void StartUseAction() override;

	virtual bool StopUseAction() override;

	virtual bool IsCanUseAction() const override;
	// ~IItem

	void SpawnProjectile();

private:
	CInterfaceComponent* m_pInterfaceComponent;

	bool m_bIsShooting = false;
	float m_fireSpeed = 1.0f;
	float m_currentFireTime = 0.0f;

	Schematyc::EntityClassName m_projectileClassname = "";
};