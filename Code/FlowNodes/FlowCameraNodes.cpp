#include "StdAfx.h"

#include <CryGame/IGameFramework.h>
#include <CryFlowGraph/IFlowBaseNode.h>

#include <CryEntitySystem/IEntitySystem.h>

#include <Core/Game.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include "Components/Camera/CameraController.h"

/// ------------------ -SPAWN PLAYER CAMERA ---------------------
class CSpawnPlayerCamera final
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
	CSpawnPlayerCamera(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSpawnPlayerCamera(pActInfo);
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

				if (Game::CGame::GetInstance()->SpawnCamera(playerClassName, entityId))
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

/// ------------------ ~SPAWN PLAYER CAMERA ---------------------

class CActivateCameraNode final
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
	CActivateCameraNode(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CActivateCameraNode(pActInfo);
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
		config.nFlags |= EFLN_TARGET_ENTITY;
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
				IEntity* cameraEntity = pActInfo->pEntity;
				if (Cry::DefaultComponents::CCameraComponent* cameraComponent = cameraEntity->GetComponent<Cry::DefaultComponents::CCameraComponent>())
				{
					cameraComponent->Activate();
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

class CSetFirstPersonState final
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
	CSetFirstPersonState(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSetFirstPersonState(pActInfo);
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
				if (const IEntity* pCameraEntity = Game::CGame::GetInstance()->GetCameraControllerEntity())
				{
					if (CCameraControllerComponent* cameraComponent = pCameraEntity->GetComponent<CCameraControllerComponent>())
					{
						cameraComponent->SetFirstPersonState();
						ActivateOutput(pActInfo, eOP_Done, true);
					}
					else
					{
						ActivateOutput(pActInfo, eOP_Done, false);
					}
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


class CSetThirdPersonState final
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
	CSetThirdPersonState(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSetThirdPersonState(pActInfo);
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
				if (const IEntity* pCameraEntity = Game::CGame::GetInstance()->GetCameraControllerEntity())
				{
					if (CCameraControllerComponent* cameraComponent = pCameraEntity->GetComponent<CCameraControllerComponent>())
					{
						cameraComponent->SetThirdPersonState();
						ActivateOutput(pActInfo, eOP_Done, true);
					}
					else
					{
						ActivateOutput(pActInfo, eOP_Done, false);
					}
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

class CSetCinematicState final
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
	CSetCinematicState(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CSetCinematicState(pActInfo);
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
				if (const IEntity* pCameraEntity = Game::CGame::GetInstance()->GetCameraControllerEntity())
				{
					if (CCameraControllerComponent* cameraComponent = pCameraEntity->GetComponent<CCameraControllerComponent>())
					{
						cameraComponent->SetCinematicState();
						ActivateOutput(pActInfo, eOP_Done, true);
					}
					else
					{
						ActivateOutput(pActInfo, eOP_Done, false);
					}
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

class CActivatePlayerCamera final
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
	CActivatePlayerCamera(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CActivatePlayerCamera(pActInfo);
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
				if (const IEntity* pCameraEntity = Game::CGame::GetInstance()->GetCameraControllerEntity())
				{
					if (Cry::DefaultComponents::CCameraComponent* cameraComponent = pCameraEntity->GetComponent<Cry::DefaultComponents::CCameraComponent>())
					{
						cameraComponent->Activate();
						ActivateOutput(pActInfo, eOP_Done, true);
					}
					else
					{
						ActivateOutput(pActInfo, eOP_Done, false);
					}
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

REGISTER_FLOW_NODE("Camera:SpawnPlayerCamera", CSpawnPlayerCamera);
REGISTER_FLOW_NODE("Camera:ActivateCamera", CActivateCameraNode);
REGISTER_FLOW_NODE("Camera:SetFirstPersonState", CSetFirstPersonState);
REGISTER_FLOW_NODE("Camera:SetThirdPersonState", CSetThirdPersonState);
REGISTER_FLOW_NODE("Camera:SetCinematicState", CSetCinematicState);
REGISTER_FLOW_NODE("Camera:ActivatePlayerCamera", CActivatePlayerCamera);