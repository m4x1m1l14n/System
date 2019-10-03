#pragma once

#include <System\Threading\EventWaitHandle.hpp>

#include <wtypes.h>

#include <vector>
#include <memory>
#include <system_error>

using namespace System;

namespace System
{
	namespace Threading
	{
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

		EventWaitHandle::operator HANDLE() const { return m_hEvent; }
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

		int EventWaitHandle::WaitOne(DWORD dwMilliseconds) const
		{
			return static_cast<int>(::WaitForSingleObject(m_hEvent, dwMilliseconds));
		}

		int EventWaitHandle::WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAny(waitHandles, INFINITE);
		}

		int EventWaitHandle::WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles, DWORD dwMilliseconds)
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

				ret = static_cast<int>(::WaitForMultipleObjects(dwCount, pHandles, FALSE, dwMilliseconds));

				delete[] pHandles;
			}

			return ret;
		}

		bool EventWaitHandle::WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles)
		{
			return WaitAll(waitHandles, INFINITE);
		}

		bool EventWaitHandle::WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles, DWORD dwMilliseconds)
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

				ret = (::WaitForMultipleObjects(dwCount, pHandles, TRUE, dwMilliseconds) == WAIT_OBJECT_0);

				delete[] pHandles;
			}

			return ret;
		}
	}
}
