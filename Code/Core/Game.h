#pragma once

#include <CryEntitySystem/IEntitySystem.h>

#include <Components/Player/PlayerComponent.h>

namespace Game
{
	class CGame
	{
		CGame() = default;
	public:
		~CGame();

		static CGame* GetInstance()
		{
			static CGame instance;
			return &instance;
		}

		const IEntity* SpawnPlayer(const char* className, EntityId spawnPointId);

		const IEntity* SpawnCamera(const char* className, EntityId spawnPointId);

		const IEntity* SpawnGameManager(const char* className);

		bool RegisterInputSystem();

		const bool SetViewPlayerCamera();

		const bool SetPlayerInput(const bool value);

		const IEntity* GetPlayerEntity() const { return gEnv->pEntitySystem->GetEntity(m_playerEntityId); }
		const IEntity* GetCameraControllerEntity() const { return gEnv->pEntitySystem->GetEntity(m_cameraControllerEntityId); }
		const IEntity* GetGameManagerEntity() const { return gEnv->pEntitySystem->GetEntity(m_gameManagerEntityId); }

		const bool IsPlayerWalking() const;

	private:
		EntityId m_playerEntityId;
		EntityId m_cameraControllerEntityId;
		EntityId m_gameManagerEntityId;

		CPlayerComponent* m_pPlayerComponent{ nullptr };
	};
}