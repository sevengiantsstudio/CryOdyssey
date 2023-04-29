//Deer Solar Games | 2021 | Lighthouse project

#include "StdAfx.h"
#include "CameraController.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>

#include <CryGame/IGameFramework.h>

#include <CryPhysics/physinterface.h>

#include <CryMath/Random.h>

#include <ViewSystem/ViewSystem.h>

#include <CryGame/GameUtils.h>

#include <Core/Game.h>
#include <Core/GameInput/GameInput.h>

using namespace Game;

static void RegisterCameraControllerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCameraControllerComponent));
		// Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::UpdateThirdPerson, "{57A3D9FE-0151-4AC5-BE44-8140294669B2}"_cry_guid, "UpdateThirdPerson");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::UpdateCinematic, "{C636E38B-7594-46C2-8BE5-DA9DCCE80BD0}"_cry_guid, "UpdateCinematic");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::UpdateInputs, "{A8CDFFAD-9B34-4C92-BBFA-9C34834AE3A3}"_cry_guid, "UpdateInputs");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::SetRightOffsetTP, "{241DFCB0-9C36-4E91-825D-07015A667AD4}"_cry_guid, "SetRightOffsetTP");
			pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
			pFunction->BindInput(1, 'ofst', "Offset");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::SetDistanceToPlayer, "{7674BE04-2D80-4CD4-A544-01688839C741}"_cry_guid, "SetDistanceToPlayer");
			pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
			pFunction->BindInput(1, 'dst', "Distance");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::SetFoV, "{162E2D87-7497-4450-95EC-AC145D64ADB2}"_cry_guid, "SetFoV");
			pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
			pFunction->BindInput(1, 'fov', "FoV");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::UpdateFirstPerson, "{0407D1FF-C517-49D5-A832-B3A4B493A706}"_cry_guid, "UpdateFirstPerson");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CCameraControllerComponent::ResetPlayerInputs, "{37BEFC3D-2D65-4D29-8F71-DE7D47F47B40}"_cry_guid, "ResetPlayerInputs");
			componentScope.Register(pFunction);
		}
		// Signals
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnFirstPersonState);
			componentScope.Register(pSignal);
		}
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnThirdPersonState);
			componentScope.Register(pSignal);
		}
		{
			auto const pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(SOnCinematicState);
			componentScope.Register(pSignal);
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterCameraControllerComponent)

CCameraControllerComponent::CCameraControllerComponent()
{

}

CCameraControllerComponent::~CCameraControllerComponent()
{
}

void CCameraControllerComponent::Initialize()
{
	if (const IEntity* pPlayerEntity = CGame::GetInstance()->GetPlayerEntity())
	{
		m_prevDesiredLoc = pPlayerEntity->GetWorldPos();
	}

	m_pCameraComponent = m_pEntity->GetComponent<Cry::DefaultComponents::CCameraComponent>();

	m_cameraDistanceTPActual = m_cameraDistanceThirdPerson;
	m_cameraFovActual = m_cameraFov;
	m_rightOffsetTPActual = m_rightOffsetThirdPerson;
}

EntityEventMask CCameraControllerComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Initialize | EEntityEvent::GameplayStarted |
		Cry::Entity::EEvent::Reset | Cry::Entity::EEvent::Update;
}

IEntityComponent::ComponentEventPriority CCameraControllerComponent::GetEventPriority() const
{
	return 15;
}

void CCameraControllerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case EEntityEvent::Initialize:
	case Cry::Entity::EEvent::Reset:
	{
		SpawnAudioListenerEntity();

	}
	break;
	case EEntityEvent::Update:
	{

	}
	break;
	}
}

void CCameraControllerComponent::OnEvent(const SInputEventSignalBase& event)
{

}

void CCameraControllerComponent::UpdateFirstPerson()
{
	if (m_pTargetEntity)
	{
		QuatT playerLoc(m_pTargetEntity->GetWorldTM());

		m_prevDeltaPlayerLoc = playerLoc.t - m_prevDeltaPlayerLoc;
		playerLoc.t += m_prevDeltaPlayerLoc;

		Matrix34 m_cameraMatrix;

		m_viewPitch += GetMousePitchDelta() * m_mousePitchSensivity + GetXiPitchDelta() * m_xiPitchSensivity;
		m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(m_pitchMinFirstPerson), DEG2RAD(m_pitchMaxFirstPerson));
		m_viewYaw += GetMouseYawDelta() * m_mouseYawSensivity - GetXiYawDelta() * m_xiYawSensivity;

		if (m_viewYaw > gf_PI)
			m_viewYaw -= gf_PI2;
		if (m_viewYaw < -gf_PI)
			m_viewYaw += gf_PI2;


		Quat quatPreTransYP = Quat(Ang3(-m_viewPitch, 0.0f, m_viewYaw));

		Vec3 eyePos = ZERO;

		if (CPlayerComponent* pPlayer = CGame::GetInstance()->GetPlayerEntity()->GetComponent<CPlayerComponent>())
		{
			const QuatT eyeLoc = playerLoc * pPlayer->GetHeadBoneLocation();
			eyePos = eyeLoc.t + m_pTargetEntity->GetForwardDir() * 0.1f + Vec3(0, 0, -0.1f);
		}

		m_cameraMatrix = Matrix34::Create(Vec3(1.0f), quatPreTransYP, eyePos);

		m_pEntity->SetLocalTM(m_cameraMatrix);

		// Rigidbody z offset
		const auto rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
		ray_hit hit;
		const bool bRayHit = gEnv->pPhysicalWorld->RayWorldIntersection(m_pTargetEntity->GetWorldPos() + Vec3(0, 0, 1.0f), Vec3(0, 0, -1) * 10.0f, ent_rigid, rayFlags, &hit, 1, m_pTargetEntity->GetPhysicalEntity());

		Quat offsetQuat = Quat::CreateIdentity();

		if (bRayHit && hit.pCollider)
		{
			IEntity* pHitEntity = static_cast<IEntity*>(hit.pCollider->GetForeignData(2));
			if (pHitEntity)
			{
				//float rigidRotZ = pHitEntity->GetWorldRotation().GetRotZ();
				//offsetQuat = Quat::CreateRotationZ(rigidRotZ);
				offsetQuat = pHitEntity->GetWorldRotation();

			}
		}
		Matrix34 m_newMat = Matrix34::Create(Vec3(1.0f), offsetQuat * m_pEntity->GetWorldRotation(), m_pEntity->GetWorldPos());
		m_pEntity->SetWorldTM(m_newMat);
		// ~Rigidbody z offset

		m_prevDeltaPlayerLoc = playerLoc.t;
	}
	else
	{
		m_pTargetEntity = CGame::GetInstance()->GetPlayerEntity();
	}

	UpdateAudioListener();
	UpdateCameraShake();

	m_prevDeltaPlayerLoc = m_pTargetEntity != nullptr ? m_pTargetEntity->GetWorldPos() : CGame::GetInstance()->GetPlayerEntity()->GetWorldPos();
}

void CCameraControllerComponent::UpdateThirdPerson()
{
	if (m_pTargetEntity)
	{
		const float frameTime = gEnv->pTimer->GetFrameTime();

		m_viewPitch += GetMousePitchDelta() * m_mousePitchSensivity + GetXiPitchDelta() * m_xiPitchSensivity;
		m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(m_pitchMinThirdPerson), DEG2RAD(m_pitchMaxThirdPerson));
		m_viewYaw += GetMouseYawDelta() * m_mouseYawSensivity - GetXiYawDelta() * m_xiYawSensivity;

		if (m_viewYaw > gf_PI)
			m_viewYaw -= gf_PI2;
		if (m_viewYaw < -gf_PI)
			m_viewYaw += gf_PI2;

		Quat desiredRot = Quat(Ang3(m_viewPitch, 0.0f, m_viewYaw));

		Interpolate(m_rightOffsetThirdPerson, m_rightOffsetTPActual, 7, gEnv->pTimer->GetFrameTime());
		Vec3 rightOffset = GetEntity()->GetForwardDir().cross(Vec3(0.f, 0.f, 1.f)).GetNormalized() * m_rightOffsetThirdPerson;

		const Vec3 armOrigin = m_pTargetEntity->GetWorldPos() + m_armOrigin + rightOffset;

		Vec3 desiredLoc = armOrigin;

		desiredLoc = Vec3::CreateLerp(m_prevDesiredLoc, desiredLoc, frameTime * 3.9f);

		Interpolate(m_cameraDistanceThirdPerson, m_cameraDistanceTPActual, 3.0f, frameTime);

		desiredRot = Quat::CreateRotationVDir((armOrigin - (armOrigin + (desiredRot * (FORWARD_DIRECTION * m_cameraDistanceThirdPerson)))).GetNormalizedSafe());
		
		m_prevDesiredLoc = desiredLoc;

		desiredLoc -= desiredRot.GetColumn1() * m_cameraDistanceThirdPerson;

		Matrix34 m_cameraMatrix = Matrix34::Create(Vec3(1.0f), desiredRot, desiredLoc);

		m_pEntity->SetLocalTM(m_cameraMatrix);

		if (m_pCameraComponent)
		{
			Interpolate(m_cameraFov, m_cameraFovActual, 3, frameTime);
			m_pCameraComponent->SetFieldOfView(CryTransform::CAngle::FromDegrees(m_cameraFov));
		}	
	}
	else
	{
		m_pTargetEntity = CGame::GetInstance()->GetPlayerEntity();
	}

	UpdateAudioListener();
	UpdateCameraShake();	
}

void CCameraControllerComponent::UpdateCinematic()
{
	if (m_pCameraRootEntity)
	{
		Vec3 pos = Vec3::CreateLerp(m_pEntity->GetPos(), m_pCameraRootEntity->GetPos(), gEnv->pTimer->GetFrameTime() * followMoveCameraSpeed);
		Quat rot = Quat::CreateSlerp(m_pEntity->GetRotation(), m_pCameraRootEntity->GetRotation(), gEnv->pTimer->GetFrameTime() * followRotateCameraSpeed);

		Matrix34 camMatrix = Matrix34::Create(Vec3(1.0f), rot, pos);
		m_pEntity->SetLocalTM(camMatrix);
	}
}

void CCameraControllerComponent::SpawnAudioListenerEntity()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(string().Format("schematyc::entities::audio_listener"));
	spawnParams.sName = "AUDIO LISTENER";
	spawnParams.vPosition = ZERO;
	spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;

	if (IEntity* pAudioListener = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		pAudioListener->GetOrCreateComponent<Cry::Audio::DefaultComponents::CListenerComponent>();
		m_pAudioListenerEntity = pAudioListener;
		CryLog("Create Audio Listener");
	}
}

void CCameraControllerComponent::SetFirstPersonState()
{
	if (Schematyc::IObject* pObject = m_pEntity->GetSchematycObject())
	{
		pObject->ProcessSignal(SOnFirstPersonState(), GetGUID());
	}
}

void CCameraControllerComponent::SetThirdPersonState()
{
	if (Schematyc::IObject* pObject = m_pEntity->GetSchematycObject())
	{
		pObject->ProcessSignal(SOnThirdPersonState(), GetGUID());
	}
}

void CCameraControllerComponent::SetCinematicState()
{
	if (Schematyc::IObject* pObject = m_pEntity->GetSchematycObject())
	{
		pObject->ProcessSignal(SOnCinematicState(), GetGUID());
	}
}


// Input methods
void CCameraControllerComponent::UpdateInputs()
{
	float cl_mouseSensitivity = 3.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivityX = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mouseYawSensivity);
	float mouseSensitivityY = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mousePitchSensivity);

	// Add the yaw delta.
	m_mouseYawDelta -= CGameInput::GetInstance()->GetMouseDelta().x * mouseSensitivityX;

	float invertYAxis = m_mouseInvertPitch ? -1.0f : 1.0f;
	m_mousePitchDelta += CGameInput::GetInstance()->GetMouseDelta().y * mouseSensitivityY * invertYAxis;

	// Gamepad input
	const Vec3 xiLookDelta = CGameInput::GetInstance()->GetXiLookDelta();

	float xiLookX = crymath::abs(xiLookDelta.x) * (crymath::abs(xiLookDelta.x) * xiLookDelta.x);
	m_xiYawDelta = DEG2RAD(xiLookX);

	float xiLookY = crymath::abs(xiLookDelta.y) * (crymath::abs(xiLookDelta.y) * xiLookDelta.y);
	m_xiPitchDelta = DEG2RAD(xiLookY * -1.0f);

	m_lastPitchDelta = m_mousePitchDelta + m_xiPitchDelta;
	m_lastYawDelta = m_mouseYawDelta + m_xiYawDelta;

	m_lastMousePitchDelta = m_mousePitchDelta;
	m_lastMouseYawDelta = m_mouseYawDelta;
	m_lastXiPitchDelta = m_xiPitchDelta;
	m_lastXiYawDelta = m_xiYawDelta;

	// Circle of life!
	m_mousePitchDelta = m_mouseYawDelta = 0.0f;
}

void CCameraControllerComponent::UpdateAudioListener()
{
	if (m_pAudioListenerEntity)
	{
		Vec3 playerDir = (m_pTargetEntity->GetWorldPos() + Vec3(0, 0, 1.75f)) - m_pEntity->GetWorldPos();
		//Vec3 listenerPos = m_pEntity->GetWorldPos() + playerDir * 0.5f;
		Vec3 listenerPos = m_pTargetEntity->GetWorldPos() + Vec3(0, 0, 1.65f);
		Quat listenerRot = m_pEntity->GetRotation();

		m_pAudioListenerEntity->SetPos(listenerPos);
		m_pAudioListenerEntity->SetRotation(listenerRot);
	}
}

void CCameraControllerComponent::SetDistanceToPlayer(const float val)
{
	m_cameraDistanceTPActual = val;
}

void CCameraControllerComponent::SetRightOffsetTP(float val)
{
	m_rightOffsetTPActual = val;
}

void CCameraControllerComponent::SetFoV(const float val)
{
	m_cameraFovActual = val;
}

void CCameraControllerComponent::UpdateCameraShake()
{
	Ang3 rotation;
	Ang3 rotationNoise;

	float noiseAmpMult = m_shakeParam.amplitudeMult * m_shakeParam.noiseAmpMult;
	float t = m_shakeParam.timeOffset;

	m_shakeParam.phase = Vec3((t + 15.0f) * m_shakeParam.frequency.x,
		(t + 55.1f) * m_shakeParam.frequency.y,
		(t + 101.2f) * m_shakeParam.frequency.z);
	m_shakeParam.phaseNoise = Vec3((t + 70.0f) * m_shakeParam.frequency.x * m_shakeParam.noiseFreqMult,
		(t + 10.0f) * m_shakeParam.frequency.y * m_shakeParam.noiseFreqMult,
		(t + 30.5f) * m_shakeParam.frequency.z * m_shakeParam.noiseFreqMult);

	const float phaseDelta = gEnv->pTimer->GetFrameStartTime(ITimer::ETIMER_GAME).GetSeconds() * 0.25f;


	m_shakeParam.phase += m_shakeParam.frequency * phaseDelta;
	m_shakeParam.phaseNoise += m_shakeParam.frequency * m_shakeParam.noiseFreqMult * phaseDelta;

	rotation.x = m_pNoiseGen.Noise1D(m_shakeParam.phase.x) * m_shakeParam.amplitude.x * m_shakeParam.amplitudeMult;
	rotationNoise.x = m_pNoiseGen.Noise1D(m_shakeParam.phaseNoise.x) * m_shakeParam.amplitude.x * noiseAmpMult;

	rotation.y = m_pNoiseGen.Noise1D(m_shakeParam.phase.y) * m_shakeParam.amplitude.y * m_shakeParam.amplitudeMult;
	rotationNoise.y = m_pNoiseGen.Noise1D(m_shakeParam.phaseNoise.y) * m_shakeParam.amplitude.y * noiseAmpMult;

	rotation.z = m_pNoiseGen.Noise1D(m_shakeParam.phase.z) * m_shakeParam.amplitude.z * m_shakeParam.amplitudeMult;
	rotationNoise.z = m_pNoiseGen.Noise1D(m_shakeParam.phaseNoise.z) * m_shakeParam.amplitude.z * noiseAmpMult;

	Ang3 angles = Ang3::GetAnglesXYZ(Matrix33(m_pEntity->GetRotation())) * 180.0f / gf_PI;
	angles += rotation + rotationNoise;
	Quat rot = Quat::CreateRotationXYZ(angles * gf_PI / 180.0f);

	m_pEntity->SetRotation(rot);
}

void CCameraControllerComponent::ResetPlayerInputs()
{
	m_mouseYawDelta = 0.0f;

	m_mousePitchDelta = 0.0f;

	m_xiYawDelta = 0.0f;
	m_xiPitchDelta = 0.0f;

	m_lastPitchDelta = m_mousePitchDelta + m_xiPitchDelta;
	m_lastYawDelta = m_mouseYawDelta + m_xiYawDelta;

	m_lastMousePitchDelta = m_mousePitchDelta;
	m_lastMouseYawDelta = m_mouseYawDelta;
	m_lastXiPitchDelta = m_xiPitchDelta;
	m_lastXiYawDelta = m_xiYawDelta;
}


Ang3 CCameraControllerComponent::GetRotationDelta()
{
	return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta);
}

float CCameraControllerComponent::GetPitchDelta()
{
	return m_lastPitchDelta;
}

float CCameraControllerComponent::GetYawDelta()
{
	return m_lastYawDelta;
}

float CCameraControllerComponent::GetMousePitchDelta()
{
	return m_lastMousePitchDelta;
}

float CCameraControllerComponent::GetMouseYawDelta()
{
	return m_lastMouseYawDelta;
}

float CCameraControllerComponent::GetXiPitchDelta()
{
	return m_lastXiPitchDelta;
}

float CCameraControllerComponent::GetXiYawDelta()
{
	return m_lastXiYawDelta;
}