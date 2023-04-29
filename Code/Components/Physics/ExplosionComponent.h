//Light Deer Games | 2021-2022 | Skyscraper project

#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/ResourceTypes.h>

class CExplosionComponent final : public IEntityComponent
{
public:
	static void ReflectType(Schematyc::CTypeDesc<CExplosionComponent>& desc)
	{
		desc.SetLabel("Explosion");
		desc.SetEditorCategory("Game");
		desc.SetDescription("Explosion component.");
		desc.SetGUID("{99DB9DE1-7010-47D5-B570-6789F9CC358F}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

		desc.AddMember(&CExplosionComponent::m_radius, 'rds', "Radius", "Explosion radius", "Explosion radius", 3.f);
		desc.AddMember(&CExplosionComponent::m_impulse, 'imp', "Impulse", "Explosion impulse", "Explosion impulse", 20.f);
		desc.AddMember(&CExplosionComponent::m_direction, 'dir', "Direction", "Explosion direction", "Explosion direction", Vec3(0.f, 0.f, 1.f));
	}

	CExplosionComponent() = default;
	virtual ~CExplosionComponent() override final {}

	//IEntityComponent
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	//~IEntityComponent

	void Simulate();

private:
	float m_radius = 3.f;
	float m_impulse = 20.f;
	Vec3 m_direction = Vec3(0.f, 0.f, 1.f);
};