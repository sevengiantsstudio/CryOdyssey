#pragma once

#include <CryEntitySystem/IEntityComponent.h>

class CItemComponent;

class CPlayerInteractionComponent final : public IEntityComponent
{
public:
	enum EInteractTypes
	{
		None,
		Grab,
		Door,
		Item,
		Lever,
	};

	CPlayerInteractionComponent() = default;
	virtual ~CPlayerInteractionComponent() = default;

	//IEntityComponent
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	//~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CPlayerInteractionComponent>& desc)
	{
		desc.SetLabel("PlayerInteract");
		desc.SetEditorCategory("Player");
		desc.SetDescription("Player interact component.");
		desc.SetGUID("{4690BCAC-4D4D-4027-A6B6-6F665B712113}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });
	}

	void OnEnter();
	void OnUpdate();
	void OnLeave();

private:
	IEntity* m_InteractiveEntity = nullptr;
	IPhysicalEntity* m_pEntityAttach = nullptr;

	EInteractTypes m_interactType = EInteractTypes::None;

	bool m_bIsFocus = false;

	float m_minDistance = 2.0f;
	float m_saveDist = 0.0f;

	float m_cursorDeltaX, m_cursorDeltaY;
	Vec3 m_cursorDoorPosition = ZERO;

	Vec3 m_deltaMoveDirectionPrevFrame = ZERO;
};