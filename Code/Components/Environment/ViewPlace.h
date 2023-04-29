//#pragma once
//
//#include <CryEntitySystem/IEntityComponent.h>
//#include <CrySchematyc/Utils/SharedString.h>
//#include "Interfaces/IInteractAction.h"
//
//class CInterfaceComponent;
//
//class CViewPlaceComponent final : public IEntityComponent, public IInteractAction
//{
//public:
//	CViewPlaceComponent() = default;
//	virtual ~CViewPlaceComponent() override final = default;
//
//	static void ReflectType(Schematyc::CTypeDesc<CViewPlaceComponent>& desc)
//	{
//		desc.SetLabel("ViewPlace");
//		desc.SetEditorCategory("InteractiveEntities");
//		desc.SetDescription("Interactive entity.");
//		desc.SetGUID("{3AA3CC5A-C362-4A24-969B-EDBD04E8BDA0}"_cry_guid);
//		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
//	}
//
//protected:
//	virtual void Initialize() override;
//	virtual Cry::Entity::EventFlags GetEventMask() const override;
//	virtual void ProcessEvent(const SEntityEvent& event) override;
//
//private:
//	//IInteractAction
//	virtual void StartUseAction() override;
//
//	virtual bool StopUseAction() override;
//
//	virtual bool IsCanUseAction() const override;
//	//~IInteractAction
//
//private:
//	CInterfaceComponent* m_pInterfaceComponent;
//};