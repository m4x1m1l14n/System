#pragma once

#include <System\Threading\EventWaitHandle.hpp>

namespace System
{
	namespace Threading
	{
		class ManualResetEvent;

		typedef std::shared_ptr<ManualResetEvent> ManualResetEvent_ptr;

		class ManualResetEvent : public EventWaitHandle
		{
		public:
			ManualResetEvent();
			ManualResetEvent(bool signaled);
			ManualResetEvent(bool signaled, const std::wstring& name);
		};
	}
}