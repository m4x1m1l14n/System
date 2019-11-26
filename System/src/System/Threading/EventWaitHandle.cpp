#pragma once

#include <System\Threading\EventWaitHandle.hpp>

#if defined(_WIN32)

#include <wtypes.h>

#else

typedef void* HANDLE;
typedef uint32_t DWORD;
typedef int BOOL;

#define TRUE 1
#define FALSE 0


#endif

namespace Linux
{
	struct EventObject
	{

	};

	BOOL SetEvent(HANDLE hEvent)
	{
		if (hEvent == nullptr)
		{

		}

		BOOL fResult = FALSE;

		auto pEvent = reinterpret_cast<EventObject*>(hEvent);
		
	}

	BOOL ResetEvent(HANDLE hEvent)
	{

	}

	DWORD GetLastError()
	{

	}
}

#include <vector>
#include <memory>
#include <system_error>

using namespace System;

namespace System
{
	namespace Threading
	{
		const int EventWaitHandle::WaitTimeout = 258; // WAIT_TIMEOUT on Windows platform

		EventWaitHandle::EventWaitHandle(bool signaled, EventResetMode mode)
			: EventWaitHandle(signaled, mode, std::wstring())
		{

		}

		EventWaitHandle::EventWaitHandle(bool signaled, EventResetMode mode, const std::wstring& name)
		{
			m_hEvent = ::CreateEvent(nullptr, mode == EventResetMode::ManualReset ? TRUE : FALSE, signaled ? TRUE : FALSE, name.c_str());
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

#if defined(_WIN32)

		EventWaitHandle::operator void*() const
		{
			return m_hEvent;
		}

#endif

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
			return (WaitOne(0) == WAIT_OBJECT_0);
		}

		int EventWaitHandle::WaitOne() const
		{ 
			return WaitOne(INFINITE); 
		}

		int EventWaitHandle::WaitOne(uint32_t milliseconds) const
		{
			return static_cast<int>(::WaitForSingleObject(m_hEvent, milliseconds));
		}

		int EventWaitHandle::WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAny(waitHandles, INFINITE);
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

				ret = static_cast<int>(::WaitForMultipleObjects(dwCount, pHandles, FALSE, milliseconds));

				delete[] pHandles;
			}

			return ret;
		}

		bool EventWaitHandle::WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAll(waitHandles, INFINITE);
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

				ret = (::WaitForMultipleObjects(dwCount, pHandles, TRUE, milliseconds) == WAIT_OBJECT_0);

				delete[] pHandles;
			}

			return ret;
		}
	}
}
