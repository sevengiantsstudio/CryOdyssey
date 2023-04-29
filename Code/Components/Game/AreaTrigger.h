#pragma once

#include <CryEntitySystem/IEntityComponent.h>

#include <CryFlowGraph/IFlowSystem.h>

class CAreaTriggerComponent final : public IEntityComponent
{
public:
	// Flowgraph
	enum EFlowgraphInputPorts
	{
		eInputPorts_Enable = 0,
		eInputPorts_Disable
	};

	enum EFlowgraphOutputPorts
	{
		eOutputPorts_Enter = 0,
		eOutputPorts_Leave
	};

	static void OnTriggerFlownodeActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CEntityFlowNode* pNode);

	void ActivateFlowNodeOutput(const int portIndex, const TFlowInputData& inputData);
	// ~Flowgraph

	CAreaTriggerComponent() = default;
	virtual ~CAreaTriggerComponent() override final {};

	// IEntityComponent
	virtual void Initialize() override;
	virtual EntityEventMask GetEventMask() const override final;
	virtual void ProcessEvent(const SEntityEvent& event) override final;
	// ~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CAreaTriggerComponent>& desc)
	{
		desc.SetGUID("{ABB74C6B-28A0-481E-BD48-164A1E30FE12}"_cry_guid);
		desc.SetEditorCategory("Triggers");
		desc.SetLabel("Area Trigger");
		desc.SetIcon("icons:audio/assets/trigger.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
	}

protected:
	void CreateHelper();
	void RemoveHelper();

private:
	bool m_bActive = true;
};