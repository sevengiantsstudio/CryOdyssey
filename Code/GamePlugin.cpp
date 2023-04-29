#include "StdAfx.h"
#include "GamePlugin.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include <CrySystem/ILocalizationManager.h>

#include <Env/GameEnv.h>

CGamePlugin::~CGamePlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
	}
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");


	//Localization setup
	ILocalizationManager* pLocMan = GetISystem()->GetLocalizationManager();
	string sLocaFolderName = "Libs/Localization/";
	string sFileName;
	sFileName.Format("localization.xml");

	string locaFile = sLocaFolderName + sFileName.c_str();

	if (pLocMan->InitLocalizationData(locaFile.c_str()))
	{
		if (!gEnv->IsEditor())
		{
			pLocMan->LoadLocalizationDataByTag("init");
		}
	}
	return true;
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
	case ESYSTEM_EVENT_GAME_POST_INIT:
	{
		// Listen for client connection events, in order to create the local player
		gEnv->pGameFramework->AddNetworkedClientListener(*this);

		// Don't need to load the map in editor
		if (!gEnv->IsEditor())
		{
			// Load the example map in client server mode
			gEnv->pConsole->ExecuteString("map test_area s", false, true);
		}

		//REGISTER DRS

		//Register BehaviorTree Nodes
	}
	break;

	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
	{
		if (gEnv->pSchematyc)
		{
			gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
				stl::make_unique<Schematyc::CEnvPackage>(
					CGamePlugin::GetCID(),
					"EntityComponents",
					"SevenGiants",
					"Components",
					[](Schematyc::IEnvRegistrar& registrar)
					{
						Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);
					}
					)
			);

			gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
				stl::make_unique<Schematyc::CEnvPackage>(
					Schematyc::g_gameModuleGUID,
					"GameNodes",
					"SevenGiants",
					"Game",
					&Schematyc::RegisterGameEnvPackage
					)
			);
		}
	}
	break;	
	}
}

bool CGamePlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

	spawnParams.sName = "EditorViewEntity";

	if (IEntity* pViewEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		pViewEntity->GetNetEntity()->SetChannelId(channelId);
		pViewEntity->GetNetEntity()->BindToNetwork();

		m_pEditorViewEntity = pViewEntity;
	}

	return true;
}

bool CGamePlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	return true;
}

void CGamePlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)