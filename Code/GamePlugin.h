#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryNetwork/INetwork.h>

class CGamePlugin
	: public Cry::IEnginePlugin
	, public ISystemEventListener
	, public INetworkedClientListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "CryPolarium", "{BCC7B614-C22D-4F45-A578-A00BB040B37C}"_cry_guid)

	PLUGIN_FLOWNODE_REGISTER
	PLUGIN_FLOWNODE_UNREGISTER

	virtual ~CGamePlugin();

	// Cry::IEnginePlugin
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// INetworkedClientListener
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	static CGamePlugin* GetInstance()
	{
		return cryinterface_cast<CGamePlugin>(CGamePlugin::s_factory.CreateClassInstance().get());
	}

	const IEntity* GetEditorViewEntity() const { return m_pEditorViewEntity; }

private:
	IEntity* m_pEditorViewEntity = nullptr;
};