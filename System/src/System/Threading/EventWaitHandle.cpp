#include <System/Threading/EventWaitHandle.hpp>

#if defined(_WIN32)

#include <wtypes.h>

#else

typedef void* HANDLE;
typedef uint32_t DWORD;
typedef int BOOL;

#define TRUE 1
#define FALSE 0

namespace Linux
{
	struct EventObject
	{

	};

	HANDLE CreateEvent(void* pEventAttributes, BOOL bManualReset, BOOL bInitialState, const wchar_t* lpName)
	{
		return nullptr;
	}

	BOOL CloseHandle(HANDLE hObject)
	{
		if (hObject != nullptr)
		{
			auto pEvent = reinterpret_cast<EventObject*>(hObject);

			delete pEvent;
		}

		return TRUE;
	}

	BOOL SetEvent(HANDLE hEvent)
	{
		if (hEvent == nullptr)
		{

		}

		BOOL fResult = FALSE;

		auto pEvent = reinterpret_cast<EventObject*>(hEvent);
		
		return TRUE;
	}

	BOOL ResetEvent(HANDLE hEvent)
	{
		return TRUE;
	}

	DWORD WaitForSingleObject(HANDLE hEvent, DWORD milliseconds)
	{
		return 0;
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

				auto *pHandles = new HANDLE[dwCount];

				for (DWORD i = 0; i < dwCount; ++i)
				{
					pHandles[i] = static_cast<HANDLE>(*waitHandles[i]);
				}

				ret = static_cast<int>(WaitForMultipleObjects(dwCount, pHandles, FALSE, milliseconds));

				delete[] pHandles;
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

				auto *pHandles = new HANDLE[dwCount];
				
				for (DWORD i = 0; i < dwCount; ++i)
				{
					pHandles[i] = static_cast<HANDLE>(*waitHandles[i]);
				}

				ret = (WaitForMultipleObjects(dwCount, pHandles, TRUE, milliseconds) == EventWaitHandle::Waitobject<0>());

				delete[] pHandles;
			}

			return ret;
		}
	}
}
