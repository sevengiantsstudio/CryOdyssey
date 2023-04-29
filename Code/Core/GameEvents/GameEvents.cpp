#include "StdAfx.h"
#include "GameEvents.h"

#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>

namespace Game
{
	CGameEvents::~CGameEvents()
	{
		m_pListeners.clear();
	}

	void CGameEvents::SubscribeToEvents(IGameEventListener* pListener, const EGameEventMask& events)
	{
		if (events == EGameEvent::None)
			return;

		auto it = m_pListeners.find(pListener);

		if (it != m_pListeners.end())
		{
			it->second.Add(events);
		}
		else
		{
			m_pListeners[pListener] = events;
		}
	}

	void CGameEvents::UnsubscribeFromEvents(IGameEventListener* pListener, const EGameEventMask& events)
	{
		if (events == EGameEvent::None)
			return;

		auto it = m_pListeners.find(pListener);

		if (it != m_pListeners.end())
		{
			if (it->second == events)
			{
				m_pListeners.erase(it);
			}
			else
			{
				it->second.Remove(events);
			}
		}
	}
}