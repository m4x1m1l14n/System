#include <System/Threading/EventWaitHandle.hpp>

#if defined(_WIN32)

#include <wtypes.h>

#else

#include <pthread.h>
#include <sys/time.h>

typedef void* HANDLE;
typedef uint32_t DWORD;
typedef int BOOL;

#define TRUE 1
#define FALSE 0

using namespace System::Threading;

namespace Linux
{
	struct EventObject
	{
		pthread_mutex_t Mutex;
		pthread_cond_t Condition;
		bool Signaled;
		bool ManualReset;
	};

	HANDLE CreateEvent(void* pEventAttributes, BOOL bManualReset, BOOL bInitialState, const wchar_t* lpName)
	{
		auto pEvent = new EventObject();

		if (pthread_cond_init(&pEvent->Condition, 0) == 0)
		{
			if (pthread_mutex_init(&pEvent->Mutex, 0) == 0)
			{
				pEvent->ManualReset = bManualReset ? true : false;
				pEvent->Signaled = bInitialState ? true : false;

				return pEvent;
			}
		}

		delete pEvent;

		return nullptr;
	}

	BOOL CloseHandle(HANDLE hObject)
	{
		if (hObject != nullptr)
		{
			auto pEvent = reinterpret_cast<EventObject*>(hObject);

			pthread_mutex_destroy(&pEvent->Mutex);
			pthread_cond_destroy(&pEvent->Condition);

			delete pEvent;
		}

		return TRUE;
	}

	BOOL SetSignaledState(HANDLE hEvent, bool signaled)
	{
		if (hEvent == nullptr)
		{
			// TODO SetLastError()
			return FALSE;
		}

		BOOL bResult = FALSE;
		auto pEvent = reinterpret_cast<EventObject*>(hEvent);

		int err = pthread_mutex_lock(&pEvent->Mutex);
		if (err == 0)
		{
			pEvent->Signaled = signaled;
			
			bResult = TRUE;

			pthread_mutex_unlock(&pEvent->Mutex);
		}

		pthread_cond_broadcast(&pEvent->Condition);
		
		return bResult;
	}

	BOOL SetEvent(HANDLE hEvent)
	{
		SetSignaledState(hEvent, true);
	}

	BOOL ResetEvent(HANDLE hEvent)
	{
		SetSignaledState(hEvent, false);
	}

	DWORD WaitForSingleObject(HANDLE hEvent, DWORD milliseconds)
	{
		if (hEvent == nullptr)
		{
			return EventWaitHandle::WaitFailed;
		}

		auto pEvent = reinterpret_cast<EventObject*>(hEvent);

		int err = 0;

        if (milliseconds == 0)
		{
            err = pthread_mutex_trylock(&pEvent->Mutex);
            if (err == EBUSY)
			{
                return EventWaitHandle::WaitTimeout;
            }
        }
		else
		{
            err = pthread_mutex_lock(&pEvent->Mutex);
        }

		// Obtaining lock failed with error
		if (err != 0)
		{
			return EventWaitHandle::WaitFailed;
		}

		uint32_t result = EventWaitHandle::WaitFailed;

		// Event already signaled, no need to wait anymore
		if (pEvent->Signaled)
		{
			// We have manual reset event in signaled state
			// so do nothing
			if (pEvent->ManualReset)
			{
			
			}
			// For AutoReset event we have to reset its state on successfull wait
			else
			{
				pEvent->Signaled = false;
			}
			
			// Set successfull wait result
			result = EventWaitHandle::WaitObject0;
		}
		// Event is not signaled, so we have to wait
		else
		{
			// Infinite wait requested
			if (milliseconds == EventWaitHandle::Infinite)
			{
				// Wait until event signaled or error
				do
				{
					err = pthread_cond_wait(&pEvent->Condition, &pEvent->Mutex);
					
				} while (err == 0 && !pEvent->Signaled);
			}
			// Timed wait requested
			else
			{
				timespec ts;
				timeval tv;

				gettimeofday(&tv, nullptr);

				uint64_t nanoseconds = ((uint64_t)tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t)tv.tv_usec) * 1000;

				ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
				ts.tv_nsec = (long) (nanoseconds - ((uint64_t)ts.tv_sec) * 1000 * 1000 * 1000);

				// Wait until event signaled, timeout or error
				do
				{
					err = pthread_cond_timedwait(&pEvent->Condition, &pEvent->Mutex, &ts);
					
				} while (err == 0 && !pEvent->Signaled);
			}

			// Handle wait result
			if (err == 0)
			{
				// Reset event state if AutoReset event
				if (!pEvent->ManualReset)
				{
					pEvent->Signaled = false;
				}

				result = EventWaitHandle::WaitObject0;
			}
			else if (err == ETIMEDOUT)
			{
				result = EventWaitHandle::WaitTimeout;
			}
			else
			{
				result = EventWaitHandle::WaitFailed;
			}
		}

		err = pthread_mutex_unlock(&pEvent->Mutex);
		if (err != 0)
		{
			// TODO Dont know if this should return WaitFailed?
		}
        
        return result;
	}

	DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* pHandles, BOOL bWaitAll, DWORD milliseconds)
	{
		return 0;
	}

	DWORD GetLastError()
	{
		return 0;
	}
}

using namespace Linux;

#endif

#include <vector>
#include <memory>
#include <system_error>

using namespace System;

namespace System
{
	namespace Threading
	{
		const uint32_t EventWaitHandle::WaitTimeout = 258; // WAIT_TIMEOUT on Windows platform
		const uint32_t EventWaitHandle::WaitAbandoned = 128;
		const uint32_t EventWaitHandle::WaitFailed = static_cast<uint32_t>(-1);
		const uint32_t EventWaitHandle::WaitObject0 = 0;

		const uint32_t EventWaitHandle::Infinite = static_cast<uint32_t>(-1);

		EventWaitHandle::EventWaitHandle(bool signaled, EventResetMode mode)
			: EventWaitHandle(signaled, mode, std::wstring())
		{

		}

		EventWaitHandle::EventWaitHandle(bool signaled, EventResetMode mode, const std::wstring& name)
		{
			m_hEvent = CreateEvent(nullptr, mode == EventResetMode::ManualReset ? TRUE : FALSE, signaled ? TRUE : FALSE, name.c_str());
			if (m_hEvent == nullptr)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"CreateEvent() returned nullptr"
				);
			}
		}

		EventWaitHandle::~EventWaitHandle()
		{
			::CloseHandle(m_hEvent);
		}

		EventWaitHandle::operator void*() const
		{
			return m_hEvent;
		}

		EventWaitHandle::operator bool() const { return IsSet(); }

		void EventWaitHandle::Set()
		{
			BOOL fResult = ::SetEvent(m_hEvent);
			if (fResult == FALSE)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"SetEvent() returned FALSE"
				);
			}
		}

		void EventWaitHandle::Reset()
		{
			BOOL fResult = ::ResetEvent(m_hEvent);
			if (fResult == FALSE)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"ResetEvent() returned FALSE"
				);
			}
		}

		bool EventWaitHandle::IsSet() const
		{
			return (WaitOne(0) == EventWaitHandle::Waitobject<0>());
		}

		int EventWaitHandle::WaitOne() const
		{ 
			return WaitOne(EventWaitHandle::Infinite);
		}

		int EventWaitHandle::WaitOne(uint32_t milliseconds) const
		{
			return static_cast<int>(WaitForSingleObject(m_hEvent, milliseconds));
		}

		int EventWaitHandle::WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAny(waitHandles, EventWaitHandle::Infinite);
		}

		int EventWaitHandle::WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles, uint32_t milliseconds)
		{
			auto ret = EventWaitHandle::WaitTimeout;

			if (!waitHandles.empty())
			{
				auto dwCount = static_cast<DWORD>(waitHandles.size());

				auto pHandles = std::make_unique<HANDLE[]>(dwCount);

				for (DWORD i = 0; i < dwCount; ++i)
				{
					pHandles[i] = static_cast<void*>(*waitHandles[i]);
				}

				ret = static_cast<int>(WaitForMultipleObjects(dwCount, pHandles.get(), FALSE, milliseconds));
			}

			return ret;
		}

		bool EventWaitHandle::WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAll(waitHandles, EventWaitHandle::Infinite);
		}

		bool EventWaitHandle::WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles, uint32_t milliseconds)
		{
			auto ret = false;

			if (!waitHandles.empty())
			{
				auto dwCount = static_cast<DWORD>(waitHandles.size());

				auto pHandles = std::make_unique<HANDLE[]>(dwCount);
				
				for (DWORD i = 0; i < dwCount; ++i)
				{
					pHandles[i] = static_cast<void*>(*waitHandles[i]);
				}

				ret = (WaitForMultipleObjects(dwCount, pHandles.get(), TRUE, milliseconds) == EventWaitHandle::Waitobject<0>());
			}

			return ret;
		}
	}
}
