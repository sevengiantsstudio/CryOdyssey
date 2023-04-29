#include "StdAfx.h"
#include "InputEvents.h"

#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>

namespace Game
{
	CInputEvents::~CInputEvents()
	{
		m_pListeners.clear();
	}

	void CInputEvents::SubscribeToEvents(IInputEventGameListener* pListener, const EInputEventMask& events)
	{
		if (events == EInputEvent::None)
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

	void CInputEvents::UnsubscribeFromEvents(IInputEventGameListener* pListener, const EInputEventMask& events)
	{
		if (events == EInputEvent::None)
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