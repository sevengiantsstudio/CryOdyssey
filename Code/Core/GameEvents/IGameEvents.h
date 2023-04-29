#pragma once

#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/ResourceTypes.h>

namespace Game
{
	enum class EGameEvent
	{
		None = 0,

		SpawnPlayer						= BIT(0),
		PlayerStateMovement				= BIT(1),
		PlayerStateFly					= BIT(2),
		PlayerStateSwim					= BIT(3),
		PlayerStartInteraction			= BIT(4),
		PlayerStopInteraction			= BIT(5),
		PlayerEnterSelectEntity			= BIT(6),
		PlayerLeaveSelectEntity			= BIT(7),
	};

	typedef CEnumFlags<EGameEvent> EGameEventMask;

	struct SGameEventSignalBase
	{
		SGameEventSignalBase() :
			m_type(EGameEvent::None)
		{};

		SGameEventSignalBase(EGameEvent type) : m_type(type)
		{};

		const EGameEvent m_type;
	};

	// Spawn Player
	struct SSpawnPlayerSignal : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::SpawnPlayer;

		SSpawnPlayerSignal() :
			SGameEventSignalBase(type)
		{};
	};

	// Player State Movement
	struct SPlayerStateMovement : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerStateMovement;

		SPlayerStateMovement() :
			SGameEventSignalBase(type)
		{};
	};

	// Player State Fly
	struct SPlayerStateFly : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerStateFly;

		SPlayerStateFly() :
			SGameEventSignalBase(type)
		{};
	};
	
	// Player State Swim
	struct SPlayerStateSwim : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerStateSwim;

		SPlayerStateSwim() :
			SGameEventSignalBase(type)
		{};
	};

	// Player Start Interaction
	struct SPlayerStartInteraction : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerStartInteraction;

		SPlayerStartInteraction() :
			SGameEventSignalBase(type)
		{};
	};

	// Player Stop Interaction
	struct SPlayerStopInteraction : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerStopInteraction;

		SPlayerStopInteraction() :
			SGameEventSignalBase(type)
		{};
	};

	// Player Enter Select Interactive Entity
	struct SPlayerEnterSelectEntity : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerEnterSelectEntity;

		SPlayerEnterSelectEntity() :
			SGameEventSignalBase(type)
		{};
	};

	// Player Leave Select Interactive Entity
	struct SPlayerLeaveSelectEntity : public SGameEventSignalBase
	{
		static constexpr EGameEvent type = EGameEvent::PlayerLeaveSelectEntity;

		SPlayerLeaveSelectEntity() :
			SGameEventSignalBase(type)
		{};
	};

	static void ReflectType(Schematyc::CTypeDesc<EGameEvent>& desc)
	{
		desc.SetGUID("{AF695110-1D50-4D61-A0A5-791C2CB7D2DC}"_cry_guid);
		desc.SetLabel("Game Event");
		desc.SetDescription("A game event that can be dispatched or recieved by components");
		desc.AddConstant(EGameEvent::SpawnPlayer, "SpawnPlayer", "Spawn Player");
		desc.AddConstant(EGameEvent::PlayerStateMovement, "Player State Movement", "Player State Movement");
		desc.AddConstant(EGameEvent::PlayerStateFly, "Player State Fly", "Player State Fly");
		desc.AddConstant(EGameEvent::PlayerStateSwim, "Player State Swim", "Player State Swim");
		desc.AddConstant(EGameEvent::PlayerStartInteraction, "Player Start Interaction", "Player Start Interaction");
		desc.AddConstant(EGameEvent::PlayerStopInteraction, "Player Stop Interaction", "Player Stop Interaction");
		desc.AddConstant(EGameEvent::PlayerEnterSelectEntity, "Player Enter Select Interactive Entity", "Player Enter Select Interactive Entity");
		desc.AddConstant(EGameEvent::PlayerLeaveSelectEntity, "Player Leave Select Interactive Entity", "Player Leave Select Interactive Entity");
	}

	static void ReflectType(Schematyc::CTypeDesc<SSpawnPlayerSignal>& desc)
	{
		desc.SetGUID("{1EE837C0-24BA-400C-A27F-1DD186F84927}"_cry_guid);
		desc.SetLabel("OnSpawnPlayer");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerStateMovement>& desc)
	{
		desc.SetGUID("{30B400A6-A357-4983-985B-6A5F692C5ED9}"_cry_guid);
		desc.SetLabel("OnPlayerStateMovement");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerStateFly>& desc)
	{
		desc.SetGUID("{64E5E3B4-BB81-4BFA-BAB3-2566579AFC02}"_cry_guid);
		desc.SetLabel("OnPlayerStateFly");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerStateSwim>& desc)
	{
		desc.SetGUID("{39F95849-FB14-4DA7-950C-D6B13E928EB5}"_cry_guid);
		desc.SetLabel("OnPlayerStateSwim");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerStartInteraction>& desc)
	{
		desc.SetGUID("{D58D22F7-68D2-4F13-9F88-3777FA0E1B8D}"_cry_guid);
		desc.SetLabel("OnPlayerStartInteraction");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerStopInteraction>& desc)
	{
		desc.SetGUID("{5C7A44B3-6EB0-4B56-BBC3-ED63C84644C8}"_cry_guid);
		desc.SetLabel("OnPlayerStopInteraction");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerEnterSelectEntity>& desc)
	{
		desc.SetGUID("{FD584A7A-9529-4ABD-AD5E-9D524BB1C42F}"_cry_guid);
		desc.SetLabel("OnPlayerEnterSelectEntity");
	}

	static void ReflectType(Schematyc::CTypeDesc<SPlayerLeaveSelectEntity>& desc)
	{
		desc.SetGUID("{D42D19E3-98D5-4726-A5D6-2B00A4ECFA32}"_cry_guid);
		desc.SetLabel("OnPlayerLeaveSelectEntity");
	}

	class IGameEventListener
	{
	public:
		virtual IEntity* GetEntity() const = 0;
		virtual void OnEvent(const SGameEventSignalBase& event) {};
	};
}