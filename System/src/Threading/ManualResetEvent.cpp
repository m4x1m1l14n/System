#include <System/Threading/ManualResetEvent.hpp>

namespace System
{
	namespace Threading
	{
		ManualResetEvent::ManualResetEvent()
			: ManualResetEvent(false)
		{}

		ManualResetEvent::ManualResetEvent(bool signaled)
			: ManualResetEvent(signaled, std::wstring())
		{}

		ManualResetEvent::ManualResetEvent(bool signaled, const std::wstring & name)
			: EventWaitHandle(signaled, EventResetMode::ManualReset, name)
		{
		}
	}
}
