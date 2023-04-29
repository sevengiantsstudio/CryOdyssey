#include "StdAfx.h"
#include "Game.h"

#include "GamePlugin.h"

#include <Core/GameInput/GameInput.h>

#include "Components/InterfaceComponent.h"
#include "Interfaces/IPawn.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

namespace Game
{
	CGame::~CGame()
	{
	}

	const IEntity* CGame::SpawnPlayer(const char* className, EntityId spawnPointId)
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(string().Format("schematyc::entities::%s", className));

		spawnParams.sName = "PlayerEntity";

		spawnParams.id = LOCAL_PLAYER_ENTITY_ID;
		spawnParams.nFlags |= ENTITY_FLAG_LOCAL_PLAYER | ENTITY_FLAG_TRIGGER_AREAS;

		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			if (!gEnv->IsEditor())
			{
				if (IEntity* pSpawnPointEntity = gEnv->pEntitySystem->GetEntity(spawnPointId))
				{
					pPlayerEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), pSpawnPointEntity->GetWorldRotation(), pSpawnPointEntity->GetWorldPos()));
				}
				else
				{
					pPlayerEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), Quat::CreateIdentity(), Vec3(ZERO)));
				}
			}
			else
			{
				if (const IEntity* pEditorViewEntity = CGamePlugin::GetInstance()->GetEditorViewEntity())
				{
					pPlayerEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), pEditorViewEntity->GetWorldRotation(), pEditorViewEntity->GetWorldPos()));
				}
			}


			m_playerEntityId = pPlayerEntity->GetId();

			m_pPlayerComponent = pPlayerEntity->GetComponent<CPlayerComponent>();

			return pPlayerEntity;
		}
		return nullptr;
	}

	const IEntity* CGame::SpawnCamera(const char* className, EntityId spawnPointId)
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(string().Format("schematyc::entities::%s", className));

		spawnParams.sName = "CameraController";
		spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;

		if (IEntity* pCameraEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			if (!gEnv->IsEditor())
			{
				if (IEntity* pSpawnPointEntity = gEnv->pEntitySystem->GetEntity(spawnPointId))
				{
					pCameraEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), pSpawnPointEntity->GetWorldRotation(), pSpawnPointEntity->GetWorldPos()));
				}
				else
				{
					pCameraEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), Quat::CreateIdentity(), Vec3(ZERO)));
				}
			}
			else
			{
				if (const IEntity* pEditorViewEntity = CGamePlugin::GetInstance()->GetEditorViewEntity())
				{
					pCameraEntity->SetWorldTM(Matrix34::Create(Vec3(1.0f), pEditorViewEntity->GetWorldRotation(), pEditorViewEntity->GetWorldPos()));
				}
			}


			m_cameraControllerEntityId = pCameraEntity->GetId();
			return pCameraEntity;
		}
		return nullptr;
	}

	const IEntity* CGame::SpawnGameManager(const char* className)
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(string().Format("schematyc::entities::%s", className));

		spawnParams.sName = "GameManager";
		spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;

		if (IEntity* pGameManagerEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			m_gameManagerEntityId = pGameManagerEntity->GetId();
			return pGameManagerEntity;
		}
		return nullptr;
	}

	bool CGame::RegisterInputSystem()
	{
		if (IEntity* pGameManagerEntity = gEnv->pEntitySystem->GetEntity(m_gameManagerEntityId))
		{
			CGameInput::GetInstance()->RegisterActionMaps(pGameManagerEntity);
			return true;
		}		
		return false;
	}

	const bool CGame::SetViewPlayerCamera()
	{
		if (const IEntity* playerEntity = GetPlayerEntity())
		{
			if (CInterfaceComponent* pInterfaceComp = playerEntity->GetComponent<CInterfaceComponent>())
			{
				if (IPawn* pPawn = pInterfaceComp->GetInterface<IPawn>())
				{
					pPawn->ActivateView();
					return true;
				}
			}
		}
		return false;
	}

	const bool CGame::SetPlayerInput(const bool value)
	{
		if (const IEntity* playerEntity = GetPlayerEntity())
		{
			if (CInterfaceComponent* pInterfaceComp = playerEntity->GetComponent<CInterfaceComponent>())
			{
				if (IPawn* pPawn = pInterfaceComp->GetInterface<IPawn>())
				{
					value ? pPawn->EnableInput() : pPawn->DisableInput();
					return true;
				}
			}
		}
		return false;
	}
	const bool CGame::IsPlayerWalking() const
	{
		if (m_pPlayerComponent)
		{
			return m_pPlayerComponent->IsWalking();
		}
		return false;
	}
}