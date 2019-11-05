#pragma once

#include <System/Threading/EventWaitHandle.hpp>

namespace System
{
	namespace Threading
	{
		class AutoResetEvent;

		typedef std::shared_ptr<AutoResetEvent> AutoResetEvent_ptr;

		class AutoResetEvent : public EventWaitHandle
		{
		public:
			AutoResetEvent();
			AutoResetEvent(bool signaled);
			AutoResetEvent(bool signaled, const std::wstring& name);
		};
	}
}
