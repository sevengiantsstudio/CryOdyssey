#pragma once

#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/ResourceTypes.h>

namespace Game
{
	enum class EInputEvent
	{
		None = 0,

		Interact			= BIT(0),
		Running				= BIT(1),
		Crouched			= BIT(2),
		Fire1				= BIT(3),
		Fire2				= BIT(4),
		Jump				= BIT(5),
		Flashlight			= BIT(6),

		// Debug
		CHANGE_VIEW			= BIT(7)
	};

	typedef CEnumFlags<EInputEvent> EInputEventMask;

	struct SInputEventSignalBase
	{
		SInputEventSignalBase() :
			m_type(EInputEvent::None),
			m_activationMode(0)
		{};

		SInputEventSignalBase(EInputEvent type) : 
			m_type(type),
			m_activationMode(0)
		{};

		SInputEventSignalBase(EInputEvent type, int activationMode) : 
			m_type(type),
			m_activationMode(activationMode)
		{};

		const EInputEvent m_type;
		int m_activationMode;
	};

	// INPUT SIGNALS

	// Interact
	struct SInteractInputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Interact;

		SInteractInputSignal() :
			SInputEventSignalBase(type)
		{};

		SInteractInputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Running
	struct SRunningInputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Running;

		SRunningInputSignal() :
			SInputEventSignalBase(type)
		{};

		SRunningInputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Crouch
	struct SCrouchInputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Crouched;

		SCrouchInputSignal() :
			SInputEventSignalBase(type)
		{};

		SCrouchInputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Fire1
	struct SFire1InputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Fire1;

		SFire1InputSignal() :
			SInputEventSignalBase(type)
		{};

		SFire1InputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Fire2
	struct SFire2InputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Fire2;

		SFire2InputSignal() :
			SInputEventSignalBase(type)
		{};

		SFire2InputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Jump
	struct SJumpInputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Jump;

		SJumpInputSignal() :
			SInputEventSignalBase(type)
		{};

		SJumpInputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// Flashlight
	struct SFlashlightInputSignal : public SInputEventSignalBase
	{
		static constexpr EInputEvent type = EInputEvent::Flashlight;

		SFlashlightInputSignal() :
			SInputEventSignalBase(type)
		{};

		SFlashlightInputSignal(int activationMode) :
			SInputEventSignalBase(type, activationMode)
		{};
	};

	// ~INPUT SIGNALS

	static void ReflectType(Schematyc::CTypeDesc<EInputEvent>& desc)
	{
		desc.SetGUID("{E668E563-0D26-47DC-A637-8A0C2FCD764F}"_cry_guid);
		desc.SetLabel("Input Event");
		desc.AddConstant(EInputEvent::Interact, "InteractInput", "Interact Input");
		desc.AddConstant(EInputEvent::Running, "RunningInput", "Running Input");
		desc.AddConstant(EInputEvent::Crouched, "CrouchedInput", "Crouched Input");
		desc.AddConstant(EInputEvent::Fire1, "Fire1Input", "Fire1 Input");
		desc.AddConstant(EInputEvent::Fire2, "Fire2Input", "Fire2 Input");
		desc.AddConstant(EInputEvent::Jump, "JumpInput", "Jump Input");
		desc.AddConstant(EInputEvent::Flashlight, "Flashlight", "Flashlight Input");
		desc.AddConstant(EInputEvent::CHANGE_VIEW, "CHANGE_VIEW", "CHANGE_VIEW");
	}

	static void ReflectType(Schematyc::CTypeDesc<SInteractInputSignal>& desc)
	{
		desc.SetGUID("{579A12CB-E585-4474-A72C-7CECA9E5874F}"_cry_guid);
		desc.SetLabel("OnInteractInput");
		desc.AddMember(&SInteractInputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SRunningInputSignal>& desc)
	{
		desc.SetGUID("{07E731D9-4FD2-43FC-91E9-C569E34F00F8}"_cry_guid);
		desc.SetLabel("OnRunningInput");
		desc.AddMember(&SRunningInputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SCrouchInputSignal>& desc)
	{
		desc.SetGUID("{46907C96-4CB0-4BBC-9860-46C52D7718E4}"_cry_guid);
		desc.SetLabel("OnCrouchedInput");
		desc.AddMember(&SCrouchInputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SFire1InputSignal>& desc)
	{
		desc.SetGUID("{8D97BD70-3138-40A6-9B2F-406EA14D800E}"_cry_guid);
		desc.SetLabel("OnFire1Input");
		desc.AddMember(&SFire1InputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SFire2InputSignal>& desc)
	{
		desc.SetGUID("{8B260003-F482-4D03-B537-7B62A7EA4D15}"_cry_guid);
		desc.SetLabel("OnFire2Input");
		desc.AddMember(&SFire2InputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SJumpInputSignal>& desc)
	{
		desc.SetGUID("{7DA450D8-5113-4D5C-A512-8CE9887F2046}"_cry_guid);
		desc.SetLabel("OnJumpInput");
		desc.AddMember(&SJumpInputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	static void ReflectType(Schematyc::CTypeDesc<SFlashlightInputSignal>& desc)
	{
		desc.SetGUID("{F6D88E34-785B-40C7-A1D9-F6A79016B532}"_cry_guid);
		desc.SetLabel("OnFlashlightInput");
		desc.AddMember(&SFlashlightInputSignal::m_activationMode, 'acmd', "Mode", "Mode", "", 0);
	}

	class IInputEventGameListener	
	{
	public:
		virtual void OnEvent(const SInputEventSignalBase& event) {};
	};
}