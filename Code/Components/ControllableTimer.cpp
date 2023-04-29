// Copyright 2019-2022 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "ControllableTimer.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>

static void RegisterControllableTimerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CControllableTimer));
		// Functions
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControllableTimer::StartTimer, "{E2CE50E4-2C25-4951-802B-4ACDBE83E145}"_cry_guid, "Start");
			pFunction->SetDescription("Starts or Resumes the timer.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControllableTimer::StopTimer, "{ED1D656D-01D5-4488-B599-4D2160B4DEC4}"_cry_guid, "Stop");
			pFunction->SetDescription("Stops the current timer. Effectively pauses the timer.");
			pFunction->BindInput(1, 'strf', "Store Fraction", "If you store a fraction when stopping a timer, the timer remembers how far it was until the next tick when it is resumed.", true);
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControllableTimer::ResetTimer, "{DC0A4ABE-43E2-4AFE-8C36-344699484171}"_cry_guid, "Reset");
			pFunction->SetDescription("Resets the timer to start from zero.");
			componentScope.Register(pFunction);
		}
		{
			auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControllableTimer::IsActive, "{3FFBAC63-2ED2-48F8-AD17-176AE6DB6C4F}"_cry_guid, "Is Active");
			pFunction->SetDescription("True if the timer is running. False otherwise.");
			componentScope.Register(pFunction);
		}

		// Signals
		{
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(STimerTickSignal));
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterControllableTimerComponent)

inline bool Serialize(Serialization::IArchive& archive, CControllableTimer::SInterval& value, const char* szName, const char* szLabel)
{
	archive(value.m_type, "type", "Type");
	archive.doc("Type");
	switch (value.m_type)
	{
	case CControllableTimer::ETimerTickType::Frames:
	{
		archive(value.m_intervalCount, "count", "Count");
		archive.doc("Count");
	} break;
	
	case CControllableTimer::ETimerTickType::Seconds:
	case CControllableTimer::ETimerTickType::Minutes:
	{
		archive(value.m_intervalFraction, "fraction", "Fraction");
		archive.doc("Fraction");
	} break;
	}
	return true;
}

CControllableTimer::CControllableTimer(SInterval&& settings, const bool& startActive)
{
	m_interval = std::move(settings);
	m_startActive = startActive;
}

CControllableTimer::~CControllableTimer()
{
	m_listeners.clear();
}

void CControllableTimer::Initialize()
{
	switch (m_interval.m_type)
	{
	case ETimerTickType::Frames:
	{
		m_tickTracker.count = 0;
	} break;

	case ETimerTickType::Seconds:
	case ETimerTickType::Minutes:
	{
		m_tickTracker.time = gEnv->pTimer->GetAsyncCurTime();
	} break;
	}

	if (m_startActive)
	{
		m_active = true;
	}
	else
	{
		m_active = false;
	}
}

EntityEventMask CControllableTimer::GetEventMask() const
{
	return EEntityEvent::Update | EEntityEvent::Reset | EEntityEvent::GameplayStarted;
}

void CControllableTimer::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case EEntityEvent::GameplayStarted:
	case EEntityEvent::Reset:
	{
		// Make sure this only get called once in case editor and game launcher decide to trigger each others events.
		// (GameplayStarted is sometimes called from editor for example)
		if (gEnv->IsEditor())
		{
			if (event.nParam[0] == 1)
			{
				Initialize();
			}
		}
		else if (!gEnv->IsEditor())
		{
			Initialize();
		}
	} break;

	case EEntityEvent::Update:
	{
		// Skip updates while editing or inactive
		if (gEnv->IsEditing() || !m_active)
		{
			return;
		}

		switch (m_interval.m_type)
		{
		case ETimerTickType::Frames:
		{
			++m_tickTracker.count;

			if (m_tickTracker.count >= m_interval.m_intervalCount * m_intervalModifier)
			{
				m_tickTracker.count = 0;
				SendSignal();
			}
		} break;
		
		case ETimerTickType::Seconds:
		case ETimerTickType::Minutes:
		{
			const float timeNow = gEnv->pTimer->GetAsyncCurTime();
			const float timeSinceLastTick = timeNow - m_tickTracker.time;
			const float tickInterval = m_interval.GetFractionInSeconds() * m_intervalModifier;

			if (timeSinceLastTick >= tickInterval)
			{
				m_tickTracker.time = timeNow - (timeSinceLastTick - tickInterval);
				SendSignal();
			}
		} break;
		}

	} break;
	}
}

void CControllableTimer::StartTimer()
{
	if (m_active == false)
	{
		switch (m_interval.m_type)
		{
		case ETimerTickType::Frames:
		{
			if (m_stopTime == 0)
			{
				m_tickTracker.count = 0;
			}
		} break;
		
		case ETimerTickType::Seconds:
		case ETimerTickType::Minutes:
		{
			if (m_stopTime == 0)
			{
				m_tickTracker.time = gEnv->pTimer->GetAsyncCurTime();
			}
			else
			{
				m_tickTracker.time = gEnv->pTimer->GetAsyncCurTime() - (m_stopTime - m_tickTracker.time);
			}
		} break;
		}

		m_active = true;
	}
}

void CControllableTimer::StopTimer(const bool& storeFraction)
{
	if (m_active)
	{
		m_active = false;

		if (storeFraction)
		{
			m_stopTime = gEnv->pTimer->GetAsyncCurTime();
		}
		else
		{
			m_stopTime = 0;
		}

	}
}

void CControllableTimer::ResetTimer()
{
	m_stopTime = 0;

	switch (m_interval.m_type)
	{
	case ETimerTickType::Frames:
	{
		m_tickTracker.count = 0;
	} break;
		
	case ETimerTickType::Seconds:
	case ETimerTickType::Minutes:
	{
		m_tickTracker.time = gEnv->pTimer->GetAsyncCurTime();
	} break;
	}
}

bool CControllableTimer::IsActive() const
{
	return m_active;
}

void CControllableTimer::SetIntervalModifier(const float& modifier)
{
	m_intervalModifier = modifier;
}

void CControllableTimer::RegisterListener(ITimerTickListener* pListener)
{
	m_listeners.emplace(pListener);
}

void CControllableTimer::UnregisterListener(ITimerTickListener* pListener)
{
	m_listeners.erase(pListener);
}

void CControllableTimer::SendSignal()
{
	for (ITimerTickListener* pListener : m_listeners)
	{
		pListener->OnTimerTick(this);
	}

	if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
	{
		pSchematycObject->ProcessSignal(STimerTickSignal(), GetGUID());
	}
}