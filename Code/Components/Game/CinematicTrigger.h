#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryPhysics/physinterface.h>

#include <CryFlowGraph/IFlowBaseNode.h>

#define DEFAULT_BOX_SIZE Vec3(1,1,1)

namespace Schematyc {
	class CEnvRegistrationScope;
}

struct SOnEnteredSignal
{
	SOnEnteredSignal() = default;
	SOnEnteredSignal(const Schematyc::ExplicitEntityId& entityId_)
		: entityId(entityId_)
	{}
	Schematyc::ExplicitEntityId entityId = Schematyc::ExplicitEntityId::Invalid;
};

struct SOnLeaveSignal
{
	SOnLeaveSignal() = default;
	SOnLeaveSignal(const Schematyc::ExplicitEntityId& entityId_)
		: entityId(entityId_)
	{}
	Schematyc::ExplicitEntityId entityId;
};

class CCinematicTrigger final : public IEntityComponent, public IEntityComponentPreviewer
{
public:
	//Flowgraph
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
	//~Flowgraph


	CCinematicTrigger() = default;
	virtual ~CCinematicTrigger() { }

	/* IEntityComponent */
	virtual void Initialize() override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	virtual EntityEventMask GetEventMask() const override;
	/* ~IEntityComponent */

	/* IEntityComponentPreviewer */
	virtual void Render(const IEntity& entity, const IEntityComponent& component, SEntityPreviewContext& context) const override;
	virtual IEntityComponentPreviewer* GetPreviewer() final { return this; }
	virtual void SerializeProperties(Serialization::IArchive& archive) final {}
	/* ~IEntityComponentPreviewer */

	static void ReflectType(Schematyc::CTypeDesc<CCinematicTrigger>& desc)
	{
		desc.SetGUID("{AA486792-817F-42D1-9952-7E2234ACDD70}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
		desc.SetIcon("icons:audio/assets/trigger.ico");
		desc.SetEditorCategory("Triggers");
		desc.SetLabel("Cinematic Trigger");

		desc.AddMember(&CCinematicTrigger::m_bEnabled, 'benb', "Enabled", "Enabled", "", true);
		desc.AddMember(&CCinematicTrigger::m_boxSize, 'bxsz', "BoxSize", "Box size", "", DEFAULT_BOX_SIZE);
		desc.AddMember(&CCinematicTrigger::m_fMaxDistance, 'mdst', "MaxDistance", "Max Distance", "", 10.f);
		desc.AddMember(&CCinematicTrigger::m_fMinDistance, 'ddst', "MinDistance", "Min Distance", "", 0.f);
		desc.AddMember(&CCinematicTrigger::m_fCheckTimer, 'ctr', "CheckTimer", "Check Timer", "", 0.25f);
		desc.AddMember(&CCinematicTrigger::m_fDelay, 'dly', "Delay", "Delay", "", 0.f);

	}

	void Enable(bool enable);

protected:
	void Setup();
	IGeometry* CreateGeometry() const;
	
	bool IsVisible();
	bool IsInRange();

private:
	IEntityTriggerComponent* m_pTriggerComponent = nullptr;
	AABB m_boundingBox;
	Vec3 m_boxSize = DEFAULT_BOX_SIZE;

	int timerId = 0;
	int timerDelayId = 1;

	bool m_bEnabled = true;
	float m_fMaxDistance = 10.f;
	float m_fMinDistance = 0;
	float m_fCheckTimer = 0.25f;
	float m_fDelay = 0.f;

	bool m_isVisible = false;
};