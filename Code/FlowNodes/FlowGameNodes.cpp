#include "StdAfx.h"

#include <CryGame/IGameFramework.h>
#include <CryFlowGraph/IFlowBaseNode.h>

#include <CryEntitySystem/IEntitySystem.h>

#include <Core/Game.h>

/// -------------------- SPAWN GAME MANAGER ----------------------
class CSpawnGameManager final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
		eIP_ClassName,
	};

	enum OUTPUTS
	{
		eOP_Done = 0
	};

public:
	CSpawnGameManager(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSpawnGameManager(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Activate", _HELP("")),
			InputPortConfig<string>("ClassName", _HELP("...")),
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
				const char* gameManagerClassName = GetPortString(pActInfo, eIP_ClassName).c_str();
				if (Game::CGame::GetInstance()->SpawnGameManager(gameManagerClassName))
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

/// ------------------- ~SPAWN GAME MANAGER ----------------------

/// ------------------ REGISTER INPUT SYSTEM ---------------------
class CRegisterInputSystem final
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
	CRegisterInputSystem(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CRegisterInputSystem(pActInfo);
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
				if (Game::CGame::GetInstance()->RegisterInputSystem())
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

/// ------------------ ~REGISTER INPUT SYSTEM --------------------

REGISTER_FLOW_NODE("Game:SpawnGameManager", CSpawnGameManager);
REGISTER_FLOW_NODE("Game:RegisterInputSystem", CRegisterInputSystem);