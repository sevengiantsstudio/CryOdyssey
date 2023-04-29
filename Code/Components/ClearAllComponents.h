//Light Deer Games | 2021-2022 | Skyscraper project

#pragma once

#include <CryEntitySystem/IEntitySystem.h>

class CClearAllComponents final : public IEntityComponent
{
public:
	CClearAllComponents() = default;
	virtual ~CClearAllComponents() = default;

	virtual void Initialize() override
	{
		m_pEntity->RemoveAllComponents();
	}

	static void ReflectType(Schematyc::CTypeDesc<CClearAllComponents>& desc)
	{
		desc.SetGUID("{3F2EDDAF-1873-46C5-BCDE-1B249D200100}"_cry_guid);
		desc.SetEditorCategory("Utils");
		desc.SetLabel("ClearAllComponents");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });
	}
};