#pragma once

#include <Core/GameInput/IInputEvents.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/IObject.h>

namespace Game
{
	class CInputEvents
	{
		friend class CGameInput;

		CInputEvents() = default;
		CInputEvents(const CInputEvents& other) = delete;

	public:
		~CInputEvents();

		static CInputEvents* GetInstance()
		{
			static CInputEvents instance;
			return &instance;
		}

		void SubscribeToEvents(IInputEventGameListener* pListener, const EInputEventMask& events);
		void UnsubscribeFromEvents(IInputEventGameListener* pListener, const EInputEventMask& events);

	private:
		template <class T>
		void SendEvent(const T& event)
		{
			for (auto& pListener : m_pListeners)
			{
				if (pListener.second.Check(event.m_type))
				{
					pListener.first->OnEvent(event);
				}
			}
		}

	private:
		std::map<IInputEventGameListener*, EInputEventMask> m_pListeners = {};
	};
}