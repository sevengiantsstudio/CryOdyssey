#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <ICryMannequin.h>

class CPlayerAnimationComponent final : public IEntityComponent
{
public:
	CPlayerAnimationComponent() = default;
	~CPlayerAnimationComponent();

	// IEntityComponent
	virtual void Initialize() override;
	virtual EntityEventMask GetEventMask() const override final;
	virtual ComponentEventPriority GetEventPriority() const override final;
	virtual void ProcessEvent(const SEntityEvent& event) override final;
	// ~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CPlayerAnimationComponent>& desc)
	{
		desc.SetGUID("{830A9616-D2CC-449A-BA40-1DA85504D560}"_cry_guid);
		desc.SetLabel("Player Animation Controller");
		desc.SetEditorCategory("Player");
		desc.SetDescription("The player animation controller component.");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });
	}

	const QuatT GetHeadBoneLocation();

	void SetAnimationDrivenMotion(bool bSet);

	void SetPlayerMovement(bool bSet) { m_bPlayerIsMovement = bSet; }

	void SetMotionParameter(EMotionParamID motionParam, float value);
	void QueueFragmentWithId(const FragmentID& fragmentId);

	ICharacterInstance* GetPlayerCharacter() { return m_pFirstPersonCachedCharacter; }

private:
	void CharacterSetup();
	void SetPlayerEntityShadow(IEntity* pEntity, bool isThirdPerson, int characterSlot, int shadowCharacterSlot);
	void HideAllAttachmentsInShadowAndRecursion(IAttachmentManager* attachmentManager, bool hideInShadow, bool hideInRecursion);

private:
	// COMMON
	FragmentID m_activeFragmentId;

	FragmentID m_idleFragmentId;
	FragmentID m_walkFragmentId;

	const SControllerDef* m_pControllerDefinition = nullptr;
	const IAnimationDatabase* m_animDatabase = nullptr;
	std::unique_ptr<SAnimationContext> m_pAnimationContext;
	IActionController* m_pActionController = nullptr;

	_smart_ptr<IAction>	m_pActiveAction;

	Vec3 m_prevForwardDir = ZERO;
	float m_turnAngle = 0.f;

	bool m_bAnimationDrivenMotion = true;

	bool m_bPlayerIsMovement = false;
	// ~COMMON

	// Cached characters
	_smart_ptr<ICharacterInstance>     m_pFirstPersonCachedCharacter = nullptr;
	_smart_ptr<ICharacterInstance>     m_pThirdPersonCachedCharacter = nullptr;


	std::bitset<eMotionParamID_COUNT>  m_overriddenMotionParams;

	IAnimationPoseAlignerPtr m_pPoseAlignerFp;
	IAnimationPoseAlignerPtr m_pPoseAlignerTp;
};