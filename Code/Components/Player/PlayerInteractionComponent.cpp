#include "StdAfx.h"
#include "PlayerInteractionComponent.h"

#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>

#include <CryPhysics/physinterface.h>
#include <CryMath/Cry_Vector3.h>

#include <Components/InterfaceComponent.h>
#include "Interfaces/IGrabObject.h"
#include "Interfaces/IHandObject.h"
#include "Interfaces/IDoor.h"
#include "Interfaces/IInteractAction.h"
#include "Interfaces/ILever.h"
#include "Interfaces/IVehicle.h"

#include <Core/GameInput/GameInput.h>
#include <Core/GameEvents/GameEvents.h>

using namespace Game;

static void RegisterInteractComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerInteractionComponent));
		// Functions
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInteractComponent)

void CPlayerInteractionComponent::Initialize()
{
	
}

Cry::Entity::EventFlags CPlayerInteractionComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Update;
}

void CPlayerInteractionComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Update:
	{
		const CCamera& camera = gEnv->pSystem->GetViewCamera();
		m_deltaMoveDirectionPrevFrame = camera.GetPosition();
	}
	break;
	}
}

void CPlayerInteractionComponent::OnEnter()
{
	const CCamera& camera = gEnv->pSystem->GetViewCamera();

	m_deltaMoveDirectionPrevFrame = camera.GetPosition() - m_deltaMoveDirectionPrevFrame;

	// Raycast
	const auto rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
	ray_hit hit;

	const bool bRayHit = gEnv->pPhysicalWorld->RayWorldIntersection(camera.GetPosition() + m_deltaMoveDirectionPrevFrame , camera.GetViewdir() * m_minDistance, ent_all, rayFlags, &hit, 1, GetEntity()->GetPhysicalEntity());
	
	//ent_use_sync_coords
	
	//IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
	//debug->Begin("player_car", true);
	//debug->AddSphere(hit.pt, 0.15f, ColorF(0.2f, 0.7f, 0.2f), 1.5f);
	

	if (bRayHit && hit.pCollider)
	{
		IEntity* pHitEntity = static_cast<IEntity*>(hit.pCollider->GetForeignData(2));
		if (pHitEntity)
		{
			if (pHitEntity == GetEntity()) return;

			if (CInterfaceComponent* pInterfaceComp = pHitEntity->GetComponent<CInterfaceComponent>())
			{
				// GRAB OBJECT
				if (IGrabObject* pInteracGrab = pInterfaceComp->GetInterface<IGrabObject>())
				{
					m_InteractiveEntity = pHitEntity;
					m_interactType = EInteractTypes::Grab;

					if (!m_pEntityAttach)
					{
						pe_params_flags pf;
						pf.flagsOR = pef_never_affect_triggers;

						(m_pEntityAttach = gEnv->pPhysicalWorld->CreatePhysicalEntity(PE_RIGID, &pf))->AddRef();

						primitives::sphere sph;
						sph.center.zero();
						sph.r = 0.3f;

						IGeomManager* pGeomManager = gEnv->pPhysicalWorld->GetGeomManager();
						phys_geometry* pGeom = pGeomManager->RegisterGeometry(pGeomManager->CreatePrimitive(primitives::sphere::type, &sph));

						pe_geomparams gp;
						gp.flags = gp.flagsCollider = 0;
						m_pEntityAttach->AddGeometry(pGeom, &gp);

						pGeom->pGeom->Release();
						pGeomManager->UnregisterGeometry(pGeom);

						pe_simulation_params sp;
						sp.noMassDecay = 1;
						m_pEntityAttach->SetParams(&sp);
					}

					if (m_pEntityAttach)
					{
						pe_status_dynamics sd;
						m_InteractiveEntity->GetPhysicalEntity()->GetStatus(&sd);

						pe_action_add_constraint aac;

						pe_params_pos pp;
						pp.doubleBufCoords = true;
						pp.pos = hit.pt;

						m_saveDist = camera.GetPosition().GetDistance(pp.pos);

						aac.flags = constraint_no_enforcement | constraint_no_tears;
						aac.pt[0] = pp.pos;
						aac.maxPullForce = sd.mass * 100;
						aac.maxBendTorque = sd.mass * 1000;
						aac.hardnessLin = 21.0f;
						aac.id = 2022;
						aac.damping = 7.0f;
						aac.pBuddy = m_pEntityAttach;

						m_InteractiveEntity->GetPhysicalEntity()->Action(&aac, (1 - m_pEntityAttach->SetParams(&pp)) >> 31);

						pe_action_add_constraint aac2;
						aac2.flags = constraint_no_enforcement | constraint_no_tears;
						aac2.pt[0] = sd.centerOfMass;
						aac2.maxPullForce = sd.mass * 100;
						aac2.maxBendTorque = sd.mass * 1000;
						aac2.hardnessLin = 21.0f;
						aac2.id = 2023;
						aac2.damping = 7.0f;
						aac2.pBuddy = m_pEntityAttach;
						m_InteractiveEntity->GetPhysicalEntity()->Action(&aac2);
					}
				}
				// ~GRAB OBJECT

				// DOOR
				if (IDoor* pInteracDoor = pInterfaceComp->GetInterface<IDoor>())
				{
					m_InteractiveEntity = pHitEntity;
					m_interactType = EInteractTypes::Door;

					if (!m_pEntityAttach)
					{
						pe_params_flags pf;
						pf.flagsOR = pef_never_affect_triggers;

						(m_pEntityAttach = gEnv->pPhysicalWorld->CreatePhysicalEntity(PE_RIGID, &pf))->AddRef();

						primitives::sphere sph;
						sph.center.zero();
						sph.r = 0.3f;

						IGeomManager* pGeomManager = gEnv->pPhysicalWorld->GetGeomManager();
						phys_geometry* pGeom = pGeomManager->RegisterGeometry(pGeomManager->CreatePrimitive(primitives::sphere::type, &sph));

						pe_geomparams gp;
						gp.flags = gp.flagsCollider = 0;
						m_pEntityAttach->AddGeometry(pGeom, &gp);

						pGeom->pGeom->Release();
						pGeomManager->UnregisterGeometry(pGeom);

						pe_simulation_params sp;
						sp.noMassDecay = 1;
						m_pEntityAttach->SetParams(&sp);
					}

					if (m_pEntityAttach)
					{
						pe_status_dynamics sd;
						m_InteractiveEntity->GetPhysicalEntity()->GetStatus(&sd);

						pe_action_add_constraint aac;

						pe_params_pos pp;
						pp.pos = hit.pt;

						m_saveDist = camera.GetPosition().GetDistance(pp.pos);
						m_cursorDoorPosition = hit.pt;

						aac.flags = constraint_no_tears | constraint_no_enforcement;
						aac.pt[0] = pp.pos;
						aac.maxPullForce = sd.mass * 100;
						aac.maxBendTorque = sd.mass * 1000;
						aac.hardnessLin = 10.0f;
						aac.id = 2022;
						aac.pBuddy = m_pEntityAttach;

						m_InteractiveEntity->GetPhysicalEntity()->Action(&aac, (1 - m_pEntityAttach->SetParams(&pp)) >> 31);
					}
				}
				// ~DOOR

				// Lever
				if (ILever* pInteractLever = pInterfaceComp->GetInterface<ILever>())
				{
					m_InteractiveEntity = pHitEntity;
					m_interactType = EInteractTypes::Lever;

					if (!m_pEntityAttach)
					{
						pe_params_flags pf;
						pf.flagsOR = pef_never_affect_triggers;

						(m_pEntityAttach = gEnv->pPhysicalWorld->CreatePhysicalEntity(PE_RIGID, &pf))->AddRef();

						primitives::sphere sph;
						sph.center.zero();
						sph.r = 0.3f;

						IGeomManager* pGeomManager = gEnv->pPhysicalWorld->GetGeomManager();
						phys_geometry* pGeom = pGeomManager->RegisterGeometry(pGeomManager->CreatePrimitive(primitives::sphere::type, &sph));

						pe_geomparams gp;
						gp.flags = gp.flagsCollider = 0;
						m_pEntityAttach->AddGeometry(pGeom, &gp);

						pGeom->pGeom->Release();
						pGeomManager->UnregisterGeometry(pGeom);

						pe_simulation_params sp;
						sp.noMassDecay = 1;
						m_pEntityAttach->SetParams(&sp);
					}

					if (m_pEntityAttach)
					{
						pe_status_dynamics sd;
						m_InteractiveEntity->GetPhysicalEntity()->GetStatus(&sd);

						pe_action_add_constraint aac;

						pe_params_pos pp;
						pp.pos = hit.pt;

						m_saveDist = camera.GetPosition().GetDistance(pp.pos);
						m_cursorDoorPosition = hit.pt;

						aac.flags = constraint_no_tears | constraint_no_enforcement;
						aac.pt[0] = pp.pos;
						aac.maxPullForce = sd.mass * 100;
						aac.maxBendTorque = sd.mass * 1000;
						aac.hardnessLin = 10.0f;
						aac.id = 2022;
						aac.pBuddy = m_pEntityAttach;

						m_InteractiveEntity->GetPhysicalEntity()->Action(&aac, (1 - m_pEntityAttach->SetParams(&pp)) >> 31);
					}
				}
				// ~Lever


				// Interact entity action
				if (IInteractAction* pInteractable = pInterfaceComp->GetInterface<IInteractAction>())
				{
					pInteractable->StartUseAction();
				}
				//~Interact entity action

				// Vehicle interaction
				if (IVehicle* pInteractable = pInterfaceComp->GetInterface<IVehicle>())
				{
					pInteractable->StartInteraction(hit.pt - m_deltaMoveDirectionPrevFrame);
					m_InteractiveEntity = pHitEntity;

					Game::CGameEvents::GetInstance()->SendEvent(SPlayerStartInteraction());
				}
				// ~Vehicle interaction
			}
		}
	}
}

void CPlayerInteractionComponent::OnUpdate()
{
	m_cursorDeltaX = CGameInput::GetInstance()->GetMouseDelta().x;
	m_cursorDeltaY = CGameInput::GetInstance()->GetMouseDelta().y;

	if (m_InteractiveEntity == nullptr)
	{
		const CCamera& camera = gEnv->pSystem->GetViewCamera();

		const auto rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
		ray_hit hit;

		m_deltaMoveDirectionPrevFrame = camera.GetPosition() - m_deltaMoveDirectionPrevFrame;

		const bool bRayHit = gEnv->pPhysicalWorld->RayWorldIntersection(camera.GetPosition() + m_deltaMoveDirectionPrevFrame, camera.GetViewdir() * m_minDistance, ent_all, rayFlags, &hit, 1, GetEntity()->GetPhysicalEntity());

		if (bRayHit && hit.pCollider)
		{
			IEntity* pHitEntity = static_cast<IEntity*>(hit.pCollider->GetForeignData(2));
			if (pHitEntity)
			{
				if (pHitEntity == GetEntity()) return;

				if (CInterfaceComponent* pInterfaceComp = pHitEntity->GetComponent<CInterfaceComponent>())
				{
					IVehicle* pVehicle = pInterfaceComp->GetInterface<IVehicle>();
					if (m_bIsFocus == false)
					{
						if (pVehicle)
						{
							if (pVehicle->GetClosestInteractiveEntity(hit.pt))
							{
								CGameEvents::GetInstance()->SendEvent(SPlayerEnterSelectEntity());
								m_bIsFocus = true;
							}
						}
						else
						{
							CGameEvents::GetInstance()->SendEvent(SPlayerEnterSelectEntity());
							m_bIsFocus = true;
						}
					}
					else
					{
						if (pVehicle)
						{
							if (pVehicle->GetClosestInteractiveEntity(hit.pt) == false)
							{
								CGameEvents::GetInstance()->SendEvent(SPlayerLeaveSelectEntity());
								m_bIsFocus = false;
							}
						}

					}
				}
			}
			else
			{
				if (m_bIsFocus)
				{
					CGameEvents::GetInstance()->SendEvent(SPlayerLeaveSelectEntity());
					m_bIsFocus = false;
				}
			}
		}

		if (!bRayHit || !hit.pCollider)
		{
			if (m_bIsFocus)
			{
				CGameEvents::GetInstance()->SendEvent(SPlayerLeaveSelectEntity());
				m_bIsFocus = false;
			}
		}
		return;
	}

	if (m_pEntityAttach)
	{
		const CCamera& camera = gEnv->pSystem->GetViewCamera();

		const auto rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
		ray_hit hit;

		IPhysicalEntity** pSkipEntities = new IPhysicalEntity * [3];
		pSkipEntities[0] = m_pEntity->GetPhysicalEntity();
		pSkipEntities[1] = m_pEntityAttach;
		pSkipEntities[2] = m_InteractiveEntity->GetPhysicalEntity();

		if (m_interactType == EInteractTypes::Grab)
		{
			const bool bRayHit = gEnv->pPhysicalWorld->RayWorldIntersection(camera.GetPosition(), camera.GetViewdir() * m_minDistance, ent_all, rayFlags, &hit, 1, pSkipEntities, 3);

			//test
			Vec3 targetPos = camera.GetPosition() + camera.GetViewdir() * m_saveDist;


			pe_params_pos pp;

			pp.pos = targetPos;
			pp.q = Quat::CreateRotationVDir(m_pEntity->GetForwardDir());

			m_pEntityAttach->SetParams(&pp);

			pe_status_dynamics sd;
			m_InteractiveEntity->GetPhysicalEntity()->GetStatus(&sd);
		}

		if (m_interactType == EInteractTypes::Door)
		{
			//test
			//Vec3 dir = (m_InteractiveEntity->GetWorldPos() - m_pEntity->GetWorldPos()).normalized();
			//m_cursorDoorPosition.x += dir.x * m_cursorDeltaX * gEnv->pTimer->GetFrameTime();
			//m_cursorDoorPosition.y += dir.y * m_cursorDeltaY * gEnv->pTimer->GetFrameTime();



			float dot = (m_pEntity->GetWorldPos() - m_InteractiveEntity->GetWorldPos()).normalize().Dot(m_InteractiveEntity->GetRightDir());
			Vec3 cross = m_pEntity->GetForwardDir().cross(m_InteractiveEntity->GetForwardDir());
			Vec3 targetCursor = Vec3(m_cursorDeltaX, m_cursorDeltaY, 0);

			Vec3 vPushAmount = (m_pEntity->GetUpDir() * m_cursorDeltaY + m_pEntity->GetRightDir() * -m_cursorDeltaX) * gEnv->pTimer->GetFrameTime() * 0.1f;

			if (dot > -0.25f && dot < 0.25f)
			{
				m_cursorDoorPosition += m_InteractiveEntity->GetUpDir().cross(vPushAmount);
			}
			else
			{
				m_cursorDoorPosition += m_InteractiveEntity->GetForwardDir().cross(vPushAmount) * crymath::sign(dot);
			}


			IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
			debug->Begin("player_interact", true);
			debug->AddSphere(m_cursorDoorPosition, 0.15f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime() + 0.1f);

			pe_params_pos pp;
			pp.pos = m_cursorDoorPosition;

			m_pEntityAttach->SetParams(&pp);

			IPersistantDebug* pDebug = gEnv->pGameFramework->GetIPersistantDebug();
			pDebug->Begin("CHPlayerDebug", false);
			pDebug->Add2DText(string().Format("dot: %f", dot).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
			pDebug->Add2DText(string().Format("vel: %f, %f, %f", cross.x, cross.y, cross.z).c_str(), 1.5f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime());
		}

		if (m_interactType == EInteractTypes::Lever)
		{
			float dot = (m_pEntity->GetWorldPos() - m_InteractiveEntity->GetWorldPos()).normalize().Dot(m_InteractiveEntity->GetRightDir());
			Vec3 cross = m_pEntity->GetForwardDir().cross(m_InteractiveEntity->GetForwardDir());
			Vec3 targetCursor = Vec3(m_cursorDeltaX, m_cursorDeltaY, 0);

			Vec3 vPushAmount = (m_pEntity->GetUpDir() * m_cursorDeltaY + m_pEntity->GetRightDir() * -m_cursorDeltaX) * gEnv->pTimer->GetFrameTime() * 0.1f;

			if (dot > -0.25f && dot < 0.25f)
			{
				m_cursorDoorPosition += m_InteractiveEntity->GetUpDir().cross(vPushAmount);
			}
			else
			{
				m_cursorDoorPosition += m_InteractiveEntity->GetForwardDir().cross(vPushAmount) * crymath::sign(dot);
			}


			IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
			debug->Begin("player_interact", true);
			debug->AddSphere(m_cursorDoorPosition, 0.15f, ColorF(0.2f, 0.7f, 0.2f), gEnv->pTimer->GetFrameTime() + 0.1f);

			pe_params_pos pp;
			pp.pos = m_cursorDoorPosition;

			m_pEntityAttach->SetParams(&pp);
		}
	}
}

void CPlayerInteractionComponent::OnLeave()
{
	if (m_InteractiveEntity)
	{
		// Vehicle
		if (CInterfaceComponent* pInterfaceComp = m_InteractiveEntity->GetComponent<CInterfaceComponent>())
		{
			if (IVehicle* pInteractable = pInterfaceComp->GetInterface<IVehicle>())
			{
				pInteractable->StopInteraction();
				m_InteractiveEntity = nullptr;

				Game::CGameEvents::GetInstance()->SendEvent(SPlayerStopInteraction());
			}
		}
		// ~Vehicle
	}

	if (m_InteractiveEntity && m_pEntityAttach)
	{
		m_bIsFocus = false;

		if (m_interactType == EInteractTypes::Grab)
		{
			pe_action_update_constraint auc;
			auc.idConstraint = 2022;
			auc.bRemove = 1;

			m_InteractiveEntity->GetPhysicalEntity()->Action(&auc);

			pe_action_update_constraint auc2;
			auc2.idConstraint = 2023;
			auc2.bRemove = 1;

			m_InteractiveEntity->GetPhysicalEntity()->Action(&auc2);

			pe_action_attach_points aap;
			aap.pEntity = m_pEntityAttach;
			aap.nPoints = 0;

			m_InteractiveEntity->GetPhysicalEntity()->Action(&aap);

			pe_action_awake aa;
			aa.bAwake = 0;

			m_pEntityAttach->Action(&aa, gEnv->pPhysicalWorld->GetPhysVars()->lastTimeStep ? -1 : 0);

			m_pEntityAttach->Release();

			//test
			pe_action_impulse pai;
			pai.impulse = Vec3(0, 0, 0.01f);

			m_InteractiveEntity->GetPhysicalEntity()->Action(&pai);

			m_InteractiveEntity = nullptr;
		}

		if (m_interactType == EInteractTypes::Door ||
			m_interactType == EInteractTypes::Lever)
		{
			pe_action_update_constraint auc;
			auc.idConstraint = 2022;
			auc.bRemove = 1;

			m_InteractiveEntity->GetPhysicalEntity()->Action(&auc);

			pe_action_attach_points aap;
			aap.pEntity = m_pEntityAttach;
			aap.nPoints = 0;

			m_InteractiveEntity->GetPhysicalEntity()->Action(&aap);

			pe_action_awake aa;
			aa.bAwake = 0;

			m_pEntityAttach->Action(&aa, gEnv->pPhysicalWorld->GetPhysVars()->lastTimeStep ? -1 : 0);

			m_pEntityAttach->Release();

			m_InteractiveEntity = nullptr;
		}
	}

}