//Deer Solar Games | 2021 | Lighthouse project

#include "StdAfx.h"

#include <Core/Game.h>

#include <CryGame/IGameFramework.h>
#include <CryFlowGraph/IFlowBaseNode.h>

#include <CryEntitySystem/IEntitySystem.h>

/// ------------------- SPAWN PLAYER ---------------------
class CSpawnPlayer final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
		eIP_ClassName,
		eIP_SpawnPointEntity,
	};

	enum OUTPUTS
	{
		eOP_Done = 0
	};

public:
	CSpawnPlayer(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSpawnPlayer(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Activate", _HELP("...")),
			InputPortConfig<string>("ClassName", _HELP("...")),
			InputPortConfig<EntityId>("SpawnPointEntity", INVALID_ENTITYID, _HELP("...")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("Done", _HELP("")),
			{ 0 }
		};
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		switch (event)
		{
		case eFE_Initialize:
		{
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIP_Activate))
			{
				const char* playerClassName = GetPortString(pActInfo, eIP_ClassName).c_str();
				EntityId entityId = GetPortEntityId(pActInfo, eIP_SpawnPointEntity);

				if (Game::CGame::GetInstance()->SpawnPlayer(playerClassName, entityId))
				{
					ActivateOutput(pActInfo, eOP_Done, true);
				}
				else
				{
					ActivateOutput(pActInfo, eOP_Done, false);
				}
			}
		}
		}
	}
};

/// ------------------ ~SPAWN PLAYER ---------------------

/// --------------- ACTIVATE PLAYER VIEW -----------------
class CActivatePlayerView final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
	};

	enum OUTPUTS
	{
		eOP_Done = 0
	};

public:
	CActivatePlayerView(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CActivatePlayerView(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Activate", _HELP("")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("Done", _HELP("")),
			{ 0 }
		};
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		switch (event)
		{
		case eFE_Initialize:
		{
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIP_Activate))
			{
				if (Game::CGame::GetInstance()->SetViewPlayerCamera())
				{
					ActivateOutput(pActInfo, eOP_Done, true);
				}
				else
				{
					ActivateOutput(pActInfo, eOP_Done, false);
				}
			}
		}
		}
	}
};
/// --------------- ~ ACTIVATE PLAYER VIEW -----------------


/// --------------- ENABLE PLAYER INPUT -----------------
class CEnablePlayerinput final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
	};

	enum OUTPUTS
	{
		eOP_Done = 0
	};

public:
	CEnablePlayerinput(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CEnablePlayerinput(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Enable", _HELP("")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("Done", _HELP("")),
			{ 0 }
		};
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		switch (event)
		{
		case eFE_Initialize:
		{
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIP_Activate))
			{
				if (Game::CGame::GetInstance()->SetPlayerInput(true))
				{
					ActivateOutput(pActInfo, eOP_Done, true);
				}
				else
				{
					ActivateOutput(pActInfo, eOP_Done, false);
				}
			}
		}
		}
	}
};
/// --------------- ~ENABLE PLAYER INPUT -----------------

/// --------------- DISABLE PLAYER INPUT -----------------
class CDisablePlayerinput final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
	};

	enum OUTPUTS
	{
		eOP_Done = 0
	};

public:
	CDisablePlayerinput(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CDisablePlayerinput(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Disable", _HELP("")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("Done", _HELP("")),
			{ 0 }
		};
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		switch (event)
		{
		case eFE_Initialize:
		{
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIP_Activate))
			{
				if (Game::CGame::GetInstance()->SetPlayerInput(false))
				{
					ActivateOutput(pActInfo, eOP_Done, true);
				}
				else
				{
					ActivateOutput(pActInfo, eOP_Done, false);
				}
			}
		}
		}
	}
};
/// --------------- ~DISABLE PLAYER INPUT -----------------

REGISTER_FLOW_NODE("Player:SpawnPlayer", CSpawnPlayer);
REGISTER_FLOW_NODE("Player:ActivateView", CActivatePlayerView);
REGISTER_FLOW_NODE("Player:EnablePlayerInput", CEnablePlayerinput);
REGISTER_FLOW_NODE("Player:DisablePlayerInput", CDisablePlayerinput);