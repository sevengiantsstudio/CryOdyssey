#pragma once

#include <Core/GameInput/InputEvents.h>
#include <DefaultComponents/Input/InputComponent.h>

namespace Game
{
	class CGameInput final
	{
		enum class EInputFlagType
		{
			Hold = 0,
			Toggle
		};

		typedef uint8 TInputFlags;

		enum class EInputFlag
			: TInputFlags
		{
			None	 = 0,
			Left	 = BIT(0),
			Right	 = BIT(1),
			Forward  = BIT(2),
			Backward = BIT(4)
		};

	public:
		CGameInput() = default;
		~CGameInput() {};

		static CGameInput* GetInstance()
		{
			static CGameInput instance;
			return &instance;
		}

		void RegisterActionMaps(IEntity* inputListener);

		Vec3 GetMovementRequest();

		Vec3 GetMovement(const Quat& baseRotation);

		Vec3 GetXiMovement(const Quat& baseRotation);

		bool IsMovementRequested() const {
			return m_inputFlags != 0 || m_xiMovementDelta.len() != 0.0f;
		}

		const Vec3 GetMouseDelta() const { return m_mouseDelta; }
		const Vec3 GetXiLookDelta() const { return m_xiLookDelta; }

	private:
		void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

	private:
		Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;

		Vec3 m_mouseDelta{ ZERO };
		Vec3 m_xiLookDelta{ ZERO };
		Vec3 m_xiMovementDelta{ ZERO };

		TInputFlags m_inputFlags{ (TInputFlags)EInputFlag::None };
	};
}