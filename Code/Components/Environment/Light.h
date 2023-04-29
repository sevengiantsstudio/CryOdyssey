#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>
#include "Interfaces/IInteractAction.h"
#include <DefaultComponents/Lights/PointLightComponent.h>
#include <DefaultComponents/Lights/ProjectorLightComponent.h>

class CInterfaceComponent;

class CLightComponent final : public IEntityComponent, public IInteractAction
{
public:
	CLightComponent() = default;
	virtual ~CLightComponent() override final = default;

	static void ReflectType(Schematyc::CTypeDesc<CLightComponent>& desc)
	{
		desc.SetLabel("Light");
		desc.SetEditorCategory("InteractiveEntities");
		desc.SetDescription("Interactive entity.");
		desc.SetGUID("{604E8CD4-D623-4F08-BAAB-6E792381E8F2}"_cry_guid);
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

	Cry::DefaultComponents::CPointLightComponent* m_pPointLightComponent = nullptr;
	Cry::DefaultComponents::CProjectorLightComponent* m_pProjectorLightComponent = nullptr;

	bool m_bActive = false;
};