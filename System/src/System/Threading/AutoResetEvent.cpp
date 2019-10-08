#include <System\Threading\AutoResetEvent.hpp>

namespace System
{
	namespace Threading
	{
		AutoResetEvent::AutoResetEvent()
			: AutoResetEvent(false)
		{}

		AutoResetEvent::AutoResetEvent(bool signaled)
			: AutoResetEvent(signaled, std::wstring())
		{}

		AutoResetEvent::AutoResetEvent(bool signaled, const std::wstring& name)
			: EventWaitHandle(signaled, EventResetMode::AutoReset, name)
		{}
	}
}
