//Deer Solar Games | 2021 | Lighthouse project

#include "StdAfx.h"

#include <CryGame/IGameFramework.h>
#include <CryFlowGraph/IFlowBaseNode.h>

#include <CryEntitySystem/IEntitySystem.h>
#include <CrySystem/ILocalizationManager.h>

class CLocalizeString final
	: public CFlowBaseNode<eNCT_Instanced>
{
	enum INPUTS
	{
		eIP_Activate = 0,
		eIP_LocId,
	};

	enum OUTPUTS
	{
		eOP_Done = 0,
		eOP_Value,
	};

public:
	CLocalizeString(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CLocalizeString(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Activate", _HELP("")),
			InputPortConfig<string>("LocId", _HELP("")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("Done", _HELP("")),
			OutputPortConfig<string>("Value", _HELP("")),
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
				ILocalizationManager* pLocMan = GetISystem()->GetLocalizationManager();
				const string& localId = GetPortString(pActInfo, eIP_LocId);
				string outValue;
				if (pLocMan->LocalizeString(localId, outValue))
				{
					ActivateOutput(pActInfo, eOP_Value, outValue);
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

REGISTER_FLOW_NODE("Localization:LocalizeString", CLocalizeString);