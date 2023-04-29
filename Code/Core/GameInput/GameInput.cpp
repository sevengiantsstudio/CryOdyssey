#include "StdAfx.h"
#include "GameInput.h"

#include <CryGame/IGameFramework.h>

namespace Game
{
	void CGameInput::RegisterActionMaps(IEntity* inputListener)
	{
		m_pInputComponent = inputListener->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

		// Forward keyboard movement
		m_pInputComponent->RegisterAction("input", "forward", [this](int activationMode, float value) {
			HandleInputFlagChange((TInputFlags)EInputFlag::Forward, activationMode);
			});
		m_pInputComponent->BindAction("input", "forward", eAID_KeyboardMouse, EKeyId::eKI_W);
		m_pInputComponent->BindAction("input", "forward", eAID_KeyboardMouse, EKeyId::eKI_Up);
		// ~Forward keyboard movement

		// Backward keyboard movement
		m_pInputComponent->RegisterAction("input", "backward", [this](int activationMode, float value) {
			HandleInputFlagChange((TInputFlags)EInputFlag::Backward, activationMode);
			});
		m_pInputComponent->BindAction("input", "backward", eAID_KeyboardMouse, EKeyId::eKI_S);
		m_pInputComponent->BindAction("input", "backward", eAID_KeyboardMouse, EKeyId::eKI_Down);
		// ~Backward keyboard movement

		// Right keyboard movement
		m_pInputComponent->RegisterAction("input", "right", [this](int activationMode, float value) {
			HandleInputFlagChange((TInputFlags)EInputFlag::Right, activationMode);
			});
		m_pInputComponent->BindAction("input", "right", eAID_KeyboardMouse, EKeyId::eKI_D);
		m_pInputComponent->BindAction("input", "right", eAID_KeyboardMouse, EKeyId::eKI_Right);
		// ~Right keyboard movement

		// Left keyboard movement
		m_pInputComponent->RegisterAction("input", "left", [this](int activationMode, float value) {
			HandleInputFlagChange((TInputFlags)EInputFlag::Left, activationMode);
			});
		m_pInputComponent->BindAction("input", "left", eAID_KeyboardMouse, EKeyId::eKI_A);
		m_pInputComponent->BindAction("input", "left", eAID_KeyboardMouse, EKeyId::eKI_Left);
		// ~Left keyboard movement

		// Gamepad movement
		m_pInputComponent->RegisterAction("input", "gamepad_forward", [this](int activationMode, float value) {
			m_xiMovementDelta.y = value;
			});
		m_pInputComponent->BindAction("input", "gamepad_forward", eAID_XboxPad, EKeyId::eKI_XI_ThumbLY);

		m_pInputComponent->RegisterAction("input", "gamepad_right", [this](int activationMode, float value) {
			m_xiMovementDelta.x = -value;
			});
		m_pInputComponent->BindAction("input", "gamepad_right", eAID_XboxPad, EKeyId::eKI_XI_ThumbLX);
		// ~Gamepad movement

		// Mouse X
		m_pInputComponent->RegisterAction("input", "mouse_x", [this](int activationMode, float value) {
			m_mouseDelta.x = value;
			});
		m_pInputComponent->BindAction("input", "mouse_x", eAID_KeyboardMouse, EKeyId::eKI_MouseX);
		// ~Mouse X

		// Mouse Y
		m_pInputComponent->RegisterAction("input", "mouse_y", [this](int activationMode, float value) { 
			m_mouseDelta.y = value;
			});
		m_pInputComponent->BindAction("input", "mouse_y", eAID_KeyboardMouse, EKeyId::eKI_MouseY);
		// ~Mouse Y

		// Gamepad look X
		m_pInputComponent->RegisterAction("input", "gamepad_look_x", [this](int activationMode, float value) {
			m_xiLookDelta.x = value;
			});
		m_pInputComponent->BindAction("input", "gamepad_look_x", eAID_XboxPad, EKeyId::eKI_XI_ThumbRX);
		// ~Gamepad look X

		// Gamepad look Y
		m_pInputComponent->RegisterAction("input", "gamepad_look_y", [this](int activationMode, float value) {
			m_xiLookDelta.y = value;
			});
		m_pInputComponent->BindAction("input", "gamepad_look_y", eAID_XboxPad, EKeyId::eKI_XI_ThumbRY);
		// ~Gamepad look Y


		// Interact
		m_pInputComponent->RegisterAction("input", "interact", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SInteractInputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "interact", eAID_KeyboardMouse, EKeyId::eKI_E);
		m_pInputComponent->BindAction("input", "interact", eAID_XboxPad, EKeyId::eKI_XI_A);

		// Running
		m_pInputComponent->RegisterAction("input", "running", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SRunningInputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "running", eAID_KeyboardMouse, EKeyId::eKI_LShift);
		m_pInputComponent->BindAction("input", "running", eAID_XboxPad, EKeyId::eKI_XI_B);

		// Fire1
		m_pInputComponent->RegisterAction("input", "fire1", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SFire1InputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "fire1", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);

		// Fire2
		m_pInputComponent->RegisterAction("input", "fire2", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SFire2InputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "fire2", eAID_KeyboardMouse, EKeyId::eKI_Mouse2);

		// Jump
		m_pInputComponent->RegisterAction("input", "jump", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SJumpInputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "jump", eAID_KeyboardMouse, EKeyId::eKI_Space);

		// Flashlight
		m_pInputComponent->RegisterAction("input", "flashlight", [this](int activationMode, float value) {
			CInputEvents::GetInstance()->SendEvent(SFlashlightInputSignal(activationMode));
			});
		m_pInputComponent->BindAction("input", "flashlight", eAID_KeyboardMouse, EKeyId::eKI_F);
	}	

	void CGameInput::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
	{
		switch (type)
		{
		case EInputFlagType::Hold:
		{
			if (activationMode & eIS_Pressed)
			{
				m_inputFlags |= flags;
			}
			else if (activationMode & eIS_Released)
			{
				m_inputFlags &= ~flags;
			}
		}
		break;

		case EInputFlagType::Toggle:
		{
			if (activationMode & eIS_Released)
			{
				// Toggle the bit(s)
				m_inputFlags ^= flags;
			}
		}
		break;
		}
	}

	Vec3 CGameInput::GetMovementRequest()
	{
		const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();

		Vec3 movementRequest = (GetMovement(Quat(systemCamera.GetMatrix())) + GetXiMovement(Quat(systemCamera.GetMatrix()))).GetNormalized();

		return movementRequest;
	}

	Vec3 CGameInput::GetMovement(const Quat& baseRotation)
	{
		bool allowMovement = true;
		Quat quatRelativeDirection;
		Vec3 vecMovement = Vec3(ZERO);

		switch (m_inputFlags)
		{
		case (TInputFlags)EInputFlag::Forward:
			quatRelativeDirection = Quat::CreateIdentity();
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(45.0f));
			break;

		case (TInputFlags)EInputFlag::Right:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(90.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(135.0f));
			break;

		case (TInputFlags)EInputFlag::Backward:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(180.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(225.0f));
			break;

		case (TInputFlags)EInputFlag::Left:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(270.0f));
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(315.0f));
			break;

		default:
			quatRelativeDirection = Quat::CreateIdentity();
			allowMovement = false;
			break;
		}

		if (allowMovement)
			vecMovement = Vec3(baseRotation.GetFwdX(), baseRotation.GetFwdY(), 0.f).GetNormalized() * quatRelativeDirection;

		return vecMovement;
	}

	Vec3 CGameInput::GetXiMovement(const Quat& baseRotation)
	{
		if (m_xiMovementDelta.len() != 0.0f)
		{
			Vec3 vecMovement = Vec3(ZERO);
			vecMovement = Vec3(baseRotation.GetFwdX(), baseRotation.GetFwdY(), 0.f).GetNormalized() * Quat::CreateRotationVDir(m_xiMovementDelta);

			return vecMovement;
		}
		return ZERO;
	}
}