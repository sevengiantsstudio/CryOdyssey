// Copyright 2019-2022 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/MathTypes.h>

struct STimerTickSignal
{
	STimerTickSignal() {};
};

class CControllableTimer;
struct ITimerTickListener
{
	virtual void OnTimerTick(CControllableTimer* pTimer) = 0;
};

class CControllableTimer final : public IEntityComponent
{
public:
	enum class ETimerTickType : uint32
	{
		Frames,
		Seconds,
		Minutes
	};

	union UInterval
	{
		float time = 0.f;
		int count;
	};

	struct SInterval
	{
		inline float GetFractionInSeconds() const
		{
			const float intervalFraction = m_intervalFraction;
			return (m_type == ETimerTickType::Minutes)? intervalFraction * 60 : intervalFraction;
		}

		static void ReflectType(Schematyc::CTypeDesc<SInterval>& desc)
		{
			desc.SetLabel("Interval");
			desc.SetDescription("Describes the interval time and amount used by the timer.");
			desc.SetGUID("{769C6FE6-9219-4192-B77B-C0873BF9DD2E}"_cry_guid);
			desc.AddMember(&SInterval::m_type, 'type', "Type", "Type", "Type of the timer interval to base the interval count/size on.", ETimerTickType::Frames);
			desc.AddMember(&SInterval::m_intervalCount, 'icnt', "Count", "Count", "Number of interval type per signal tick.", 1);
			desc.AddMember(&SInterval::m_intervalFraction, 'ifrc', "Fraction", "Fraction", "Amount of interval type per signal tick.", 1.f);
		}
		inline bool operator==(const SInterval& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		ETimerTickType m_type = ETimerTickType::Frames;
		Schematyc::Range<1, 1000, 1, 1000, int> m_intervalCount = 1;
		Schematyc::Range<0, 1000, 0, 1000, float> m_intervalFraction = 1.0f;
	};

	CControllableTimer() = default;
	CControllableTimer(SInterval&& settings, const bool& startActive = false);
	virtual ~CControllableTimer() override;

	static void ReflectType(Schematyc::CTypeDesc<CControllableTimer>& desc)
	{
		desc.SetLabel("Controllable Timer");
		desc.SetEditorCategory("Misc");
		desc.SetDescription("Provides a signal (tick) based on the properties. Timer can be paused, resumed and reset.");
		desc.SetGUID("{BEC3421A-AEF2-4079-8F02-88923C176CD8}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&CControllableTimer::m_startActive, 'stac', "StartActive", "Start Active", "Start the timer immediately when the component is initialized.", false);
		desc.AddMember(&CControllableTimer::m_interval, 'iset', "IntervalSettings", "Interval Settings", "Settings relating to the time interval type and tick size.", SInterval());
	}

	void StartTimer();
	void StopTimer(const bool& storeFraction);
	void ResetTimer();

	bool IsActive() const;
	void SetIntervalModifier(const float& interval);

	void RegisterListener(ITimerTickListener* pListener);
	void UnregisterListener(ITimerTickListener* pListener);

protected:
	// IEntityComponent
	virtual void Initialize() override final;
	virtual EntityEventMask GetEventMask() const override final;
	virtual void ProcessEvent(const SEntityEvent& event) override final;
	// ~IEntityComponent

	void SendSignal();

private:
	bool m_active = false;
	UInterval m_tickTracker = UInterval();
	float m_stopTime = 0; // Used to account for remainder when resuming timers
	float m_intervalModifier = 1.f; // Modififer used to adjust interval timing at runtime
	std::set<ITimerTickListener*> m_listeners = {};
	
	bool m_startActive = false;
	SInterval m_interval = SInterval();
};

static void ReflectType(Schematyc::CTypeDesc<STimerTickSignal>& desc)
{
	desc.SetLabel("OnTick");
	desc.SetGUID("{0DD04500-E3B2-483F-A559-0A1B1B0BCC3F}"_cry_guid);
}

static void ReflectType(Schematyc::CTypeDesc<CControllableTimer::ETimerTickType>& desc)
{
	desc.SetLabel("Interval Type");
	desc.SetDescription("The interval type the timer uses to calculate ticks");
	desc.SetGUID("{49F05342-5ECB-49AD-8EB8-CC13858F5AF2}"_cry_guid);
	desc.AddConstant(CControllableTimer::ETimerTickType::Frames, "Frames", "Frames");
	desc.AddConstant(CControllableTimer::ETimerTickType::Seconds, "Seconds", "Seconds");
	desc.AddConstant(CControllableTimer::ETimerTickType::Minutes, "Minutes", "Minutes");
}