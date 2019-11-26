#pragma once

#include <vector>
#include <memory>
#include <exception>

namespace System
{
	namespace Threading
	{
		enum EventResetMode
		{
			AutoReset,
			ManualReset
		};

		class EventWaitHandle;

		typedef std::shared_ptr<EventWaitHandle> EventWaitHandle_ptr;

		class EventWaitHandle
		{
		public:
			static const uint32_t WaitTimeout;
			static const uint32_t WaitAbandoned;
			static const uint32_t WaitFailed;
			static const uint32_t WaitObject0;

			static const uint32_t Infinite;

			template <uint32_t N>
			static constexpr uint32_t Waitobject()
			{
				return WaitObject0 + N;
			}

		public:
			EventWaitHandle(bool signaled, EventResetMode mode);
			EventWaitHandle(bool signaled, EventResetMode mode, const std::wstring& name);

			virtual ~EventWaitHandle();

			explicit operator void*() const;

			operator bool() const;

			void Set();
			void Reset();

			bool IsSet() const;

			int WaitOne() const;
			int WaitOne(uint32_t milliseconds) const;

			static int WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles);
			static int WaitAny(const std::vector<EventWaitHandle_ptr>& waitHandles, uint32_t milliseconds);
			static bool WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles);
			static bool WaitAll(const std::vector<EventWaitHandle_ptr>& waitHandles, uint32_t milliseconds);

		protected:
			void* m_hEvent;
		};
	}
}
