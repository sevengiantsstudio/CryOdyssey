//#include "StdAfx.h"
//#include "ViewPlace.h"
//#include <CryEntitySystem/IEntitySystem.h>
//#include <CryCore/StaticInstanceList.h>
//#include <CrySchematyc/Env/IEnvRegistrar.h>
//#include <CrySchematyc/Env/Elements/EnvComponent.h>
//#include <CrySchematyc/Env/Elements/EnvFunction.h>
//#include "Components/InterfaceComponent.h"
//#include <DefaultComponents/Cameras/CameraComponent.h>
//#include "Core/PlayerController.h"
//#include "Pawns/PlayerViewOnly.h"
//
//static void RegisterViewPlaceComponent(Schematyc::IEnvRegistrar& registrar)
//{
//	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
//	{
//		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CViewPlaceComponent));
//		// Functions
//		{
//		}
//	}
//}
//CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterViewPlaceComponent)
//
//void CViewPlaceComponent::Initialize()
//{
//	m_pInterfaceComponent = m_pEntity->GetOrCreateComponent<CInterfaceComponent>();
//}
//
//Cry::Entity::EventFlags CViewPlaceComponent::GetEventMask() const
//{
//	return
//		Cry::Entity::EEvent::GameplayStarted;
//}
//
//void CViewPlaceComponent::ProcessEvent(const SEntityEvent& event)
//{
//	switch (event.event)
//	{
//	case Cry::Entity::EEvent::GameplayStarted:
//	{
//		m_pInterfaceComponent->AddInterface<IInteractAction>(this);
//	}
//	break;
//	}
//}
//
//void CViewPlaceComponent::StartUseAction()
//{
//	//Get camera entity
//	IEntityLink* pLink = m_pEntity->GetEntityLinks();
//	while (pLink)
//	{
//		if (IEntity* entity = gEnv->pEntitySystem->GetEntity(pLink->entityId))
//		{
//			CPlayerController::Get()->SetControlledPawn(entity->GetComponent<CPlayerViewOnly>());
//			entity->GetComponent<CPlayerViewOnly>()->AllowUpdateEvent();
//			return;
//		}
//		pLink = pLink->next;
//	}
//}
//
//bool CViewPlaceComponent::StopUseAction()
//{
//	return true;
//}
//
//bool CViewPlaceComponent::IsCanUseAction() const
//{
//	return true;
//}
