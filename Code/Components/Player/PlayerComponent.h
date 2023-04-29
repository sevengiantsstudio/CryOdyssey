#pragma once

#include <array>
#include <numeric>

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CryMath/Cry_Camera.h>

#include <ICryMannequin.h>
#include <CrySchematyc/Utils/EnumFlags.h>

#include <CrySchematyc/ResourceTypes.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>
#include <DefaultComponents/Physics/RagdollComponent.h>

#include <CryAISystem/IFactionMap.h>
#include <CryAISystem/Components/IEntityFactionComponent.h>

#include "Interfaces/IPawn.h"

#include "PlayerAnimationComponent.h"
#include "PlayerInteractionComponent.h"


// LOCAL PLAYER EVENTS
struct SOnSwimStateEnter
{
	SOnSwimStateEnter() = default;
};

struct SOnSwimStateExit
{
	SOnSwimStateExit() = default;
};

static void ReflectType(Schematyc::CTypeDesc<SOnSwimStateEnter>& desc)
{
	desc.SetGUID("{4EC8F4F3-6794-428A-AB28-2977EFA72A3C}"_cry_guid);
	desc.SetLabel("OnSwimStateEnter");
	desc.SetDescription("...");
}

static void ReflectType(Schematyc::CTypeDesc<SOnSwimStateExit>& desc)
{
	desc.SetGUID("{408D83A1-4559-4365-887B-DFF617B60A0A}"_cry_guid);
	desc.SetLabel("OnSwimStateExit");
	desc.SetDescription("...");
}

// ~LOCAL PLAYER EVENTS

class CInterfaceComponent;

class CPlayerComponent final : public IEntityComponent, public IPawn
{
public:

	CPlayerComponent() = default;
	virtual ~CPlayerComponent();

	// IEntityComponent
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
		desc.SetEditorCategory("Player");
		desc.SetLabel("Player Component");
		desc.SetDescription("This component for third person player implementation.");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::Schematyc,
			IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::Socket });

		desc.AddMember(&CPlayerComponent::m_walkSpeed, 'wspd', "WalkSpeed", "Walk Speed", "Walk Speed", 1.75f);
		desc.AddMember(&CPlayerComponent::m_runSpeed, 'rspd', "RunSpeed", "Run Speed", "Run Speed", 3.5f);
		desc.AddMember(&CPlayerComponent::m_crouchSpeed, 'cspd', "CrouchSpeed", "Crouch Speed", "Crouch Speed", 1.0f);
		desc.AddMember(&CPlayerComponent::m_accelerationTime, 'actm', "AccelerationTime", "Acceleration Time", "Acceleration Time", 2.0f);
		desc.AddMember(&CPlayerComponent::m_jumpPower, 'jpwr', "JumpPower", "Jump Power", "Jump Power", 3.5f);
	}

	// IPawn
	virtual void EnableInput() override;
	virtual void DisableInput() override;
	virtual void ActivateView() override;
	// ~IPawn


	void UpdateStrafeMovement();

	void UpdateSwimming();

	void SetFlyMode();

	void SetMovementMode();

	void UpdateFlying();
	
	void UpdateAnimation();

	void StartSprint();

	void StopSprint();

	void SetCrouch(bool val);

	void Jump();

	void StartInteraction();

	void StopInteraction();

	void EnableWeaponAim();

	void DisableWeaponAim();

	void UpdateWeaponAim();

	Vec3 GetMovementRequest();

	const QuatT GetHeadBoneLocation();

	const bool IsWalking() const { return m_pCharacterController->IsWalking(); }

	const bool GetAllowInput() { return m_bAllowInput; }

private:
	void SpawnFlashlight();

private:
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController = nullptr;
	CPlayerAnimationComponent* m_pAnimationController = nullptr;
	CPlayerInteractionComponent* m_pInteractionController = nullptr;

	CInterfaceComponent* m_pInterfaceComponent = nullptr;

	//Movement
	float m_walkSpeed = 3.5f;
	float m_runSpeed = 4.5f;
	float m_crouchSpeed = 1.0f;
	float m_rifleSpeed = 3.5f;

	//smooth speed
	float m_smoothSpeed;

	float m_currentSpeed = 0.0f;

	float m_accelerationTime = 3.0f;

	float m_jumpPower = 3.5f;

	Vec3 m_lastMovementRequest = Vec3(0);

	bool m_bAllowInput{ true };
};
