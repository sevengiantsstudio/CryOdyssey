// Deer Solar Games | 2021 | Lighthouse project

// Camera controller component for game view
// Writed by quantbrain

#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>

#include <Core/GameInput/IInputEvents.h>

#include <CryMath/PNoise3.h>

using namespace Game;

struct SOnFirstPersonState
{
	SOnFirstPersonState() = default;
};

struct SOnThirdPersonState
{
	SOnThirdPersonState() = default;
};

struct SOnCinematicState
{
	SOnCinematicState() = default;
};

static void ReflectType(Schematyc::CTypeDesc<SOnFirstPersonState>& desc)
{
	desc.SetGUID("{A29A752B-BF47-465B-95F8-43CFF93F1DC2}"_cry_guid);
	desc.SetLabel("OnFirstPersonState");
	desc.SetDescription("...");
}

static void ReflectType(Schematyc::CTypeDesc<SOnThirdPersonState>& desc)
{
	desc.SetGUID("{DF755032-3EDC-4B03-BD1C-A99592C4133F}"_cry_guid);
	desc.SetLabel("OnThirdPersonState");
	desc.SetDescription("...");
}

static void ReflectType(Schematyc::CTypeDesc<SOnCinematicState>& desc)
{
	desc.SetGUID("{BE423FD4-0C7D-4E0E-A066-0196D28E708D}"_cry_guid);
	desc.SetLabel("OnCinematicState");
	desc.SetDescription("...");
}


class CCameraControllerComponent final : public IEntityComponent, public IInputEventGameListener
{
public:
	CCameraControllerComponent();
	~CCameraControllerComponent();

	// IEntityComponent
	virtual void Initialize() override;
	virtual EntityEventMask GetEventMask() const override final;
	virtual ComponentEventPriority GetEventPriority() const override final;
	virtual void ProcessEvent(const SEntityEvent& event) override final;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CCameraControllerComponent>& desc)
	{
		desc.SetGUID("{22A6DE22-48A3-487E-8D45-C8FB631C64A5}"_cry_guid);
		desc.SetLabel("Camera Controller");
		desc.SetEditorCategory("Camera");
		desc.SetDescription("The camera controller component.");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });

		// Common
		desc.AddMember(&CCameraControllerComponent::m_cameraFarPlane, 'fpcm', "CameraFarPlane", "Camera Far Plane", "..", 8000.0f);

		// First Person
		desc.AddMember(&CCameraControllerComponent::m_pitchMinFirstPerson, 'fmin', "PitchMinFirstPerson", "Pitch Min First Person", "..", 15.f);
		desc.AddMember(&CCameraControllerComponent::m_pitchMaxFirstPerson, 'fmax', "PitchMaxFirstPerson", "Pitch Max First Person", "..", 85.f);

		// Third Person
		desc.AddMember(&CCameraControllerComponent::m_cameraDistanceThirdPerson, 'dsnt', "CameraDistanceThirdPerson", "Camera Distance Third Person", "..", 3.f);
		desc.AddMember(&CCameraControllerComponent::m_pitchMinThirdPerson, 'pmax', "PitchMinThirdPerson", "Pitch Min Third Person", "..", 15.f);
		desc.AddMember(&CCameraControllerComponent::m_pitchMaxThirdPerson, 'pmin', "PitchMaxThirdPerson", "Pitch Max Third Person", "..", 85.f);

		desc.AddMember(&CCameraControllerComponent::m_rightOffsetThirdPerson, 'rifs', "RightOffsetThirdPerson", "Right Offset Third Person", "..", 0.f);

		desc.AddMember(&CCameraControllerComponent::m_armOrigin, 'aorg', "ArmOrigin", "Arm Origin", "..", Vec3(0, 0, 1.75f));

		// Cinematic
		desc.AddMember(&CCameraControllerComponent::followMoveCameraSpeed, 'fmcs', "FollowMoveCameraSpeed", "Follow Move Camera Speed", "..", 1.0f);
		desc.AddMember(&CCameraControllerComponent::followRotateCameraSpeed, 'frcs', "FollowRotateCameraSpeed", "Follow Rotate Camera Speed", "..", 1.0f);
		desc.AddMember(&CCameraControllerComponent::m_cameraRootName, 'rtne', "CameraRootName", "Camera Root Name", "For Cinematic mode", "_root");
	}

	virtual void OnEvent(const SInputEventSignalBase& event) override;

	void UpdateFirstPerson();

	void UpdateThirdPerson();

	void UpdateCinematic();

	void UpdateInputs();

	void UpdateAudioListener();

	void SetDistanceToPlayer(const float val);

	void SetRightOffsetTP(float val);

	void SetFoV(const float val);

	void UpdateCameraShake();

	void ResetPlayerInputs();

	// Set STATES
	void SetFirstPersonState();

	void SetThirdPersonState();

	void SetCinematicState();

protected:
	void SpawnAudioListenerEntity();

private:

	Ang3 GetRotationDelta();

	float GetPitchDelta();

	float GetYawDelta();

	float GetMousePitchDelta();

	float GetMouseYawDelta();

	float GetXiPitchDelta();

	float GetXiYawDelta();

private:
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
	const IEntity* m_pTargetEntity = nullptr;
	IEntity* m_pAudioListenerEntity = nullptr;

	// Common
	float m_cameraFarPlane = 8000.0f;

	// First Person
	Schematyc::Range<-89, 89> m_pitchMinFirstPerson = 15.0f;
	Schematyc::Range<-89, 89> m_pitchMaxFirstPerson = 85.0f;

	// Third Person
	Vec3 m_armOrigin = ZERO;

	float m_cameraDistanceThirdPerson = 7.f;
	float m_cameraDistanceTPActual = 0.0f;

	float m_cameraFov = 47.0f;
	float m_cameraFovActual = 55.0f;

	Schematyc::Range<-89, 89> m_pitchMinThirdPerson = 15.0f;
	Schematyc::Range<-89, 89> m_pitchMaxThirdPerson = 85.0f;

	float m_rightOffsetThirdPerson = 0.0f;
	float m_rightOffsetTPActual = 0.0f;

	Vec3 m_prevDesiredLoc = ZERO;

	// Cinematic
	float followMoveCameraSpeed = 1.0f;
	float followRotateCameraSpeed = 1.0f;
	Schematyc::CSharedString m_cameraRootName = "_root";
	IEntity* m_pCameraRootEntity{ nullptr };
	// ~Cinematic

	float m_viewPitch = DEG2RAD(5.f);
	float m_viewYaw = DEG2RAD(180.0f);

	Quat m_quatLastTargetRotation{ IDENTITY };
	Quat m_quatTargetRotation{ IDENTITY };

	// Inputs
	int m_mouseInvertPitch{ 0 };

	// sensivity
	float m_mousePitchYawSensitivity = 1.0f;

	float m_mousePitchSensivity = 1.35f;
	float m_mouseYawSensivity = 1.35f;

	float m_xiPitchSensivity = 2.7f;
	float m_xiYawSensivity = 4.35f;
	// ~sensivity

	float m_lastPitchDelta{ 0.0f };

	float m_lastYawDelta{ 0.0f };

	float m_mousePitchDelta{ 0.0f };
	float m_lastMousePitchDelta{ 0.0f };

	float m_mouseYawDelta{ 0.0f };
	float m_lastMouseYawDelta{ 0.0f };

	float m_xiPitchDelta{ 0.0f };
	float m_lastXiPitchDelta{ 0.0f };

	float m_xiYawDelta{ 0.0f };
	float m_lastXiYawDelta{ 0.0f };

	// Camera shake
	struct ShakeParam
	{
		Vec3  amplitude;
		float amplitudeMult;
		Vec3  frequency;
		float frequencyMult;
		float noiseAmpMult;
		float noiseFreqMult;
		float timeOffset;

		Vec3  phase;
		Vec3  phaseNoise;

		ShakeParam()
		{
			amplitude = Vec3(1.0f, 0.35f, 1.0f);
			amplitudeMult = 1.1f;
			frequency = Vec3(1.0f, 0.35f, 1.0f);
			frequencyMult = 1.5f;
			noiseAmpMult = 2.5f;
			noiseFreqMult = 1.5f;
			timeOffset = 0.0f;
		}
	};

	ShakeParam m_shakeParam;
	CPNoise3 m_pNoiseGen;

	Vec3 m_prevDeltaPlayerLoc = ZERO;
};