///
/// Inspired by
///	https://www.codeproject.com/Articles/146617/Simple-C-Timer-Wrapper
///
#pragma once

#include <Windows.h>
#include <assert.h>

#include <helpers.h>

#include <System/TimeSpan.hpp>

namespace System
{
	namespace Timers
	{
		enum class TimerFlags : ULONG
		{
			/// <summary>
			/// By default, the callback function is queued to a non-I/O worker thread.
			/// </summary>
			ExecuteDefault = WT_EXECUTEDEFAULT,

			/// <summary>
			/// The callback function is invoked by the timer thread itself. This flag 
			/// should be used only for short tasks or it could affect other timer operations. 
			/// The callback function is queued as an APC. It should not perform alertable 
			/// wait operations.
			/// </summary>
			ExecuteInTimerThread = WT_EXECUTEINTIMERTHREAD,

			/// <summary>
			/// The callback function is queued to an I/O worker thread. This flag should 
			/// be used if the function should be executed in a thread that waits in an 
			/// alertable state. 
			/// The callback function is queued as an APC. Be sure to address reentrancy 
			/// issues if the function performs an alertable wait operation.
			/// </summary>
			ExecuteInIOThread = WT_EXECUTEINIOTHREAD,

			/// <summary>
			/// The callback function is queued to a thread that never terminates. It 
			/// does not guarantee that the same thread is used each time. This flag 
			/// should be used only for short tasks or it could affect other timer 
			/// operations. currently no worker thread is truly persistent, 
			/// although no worker thread will terminate if there are any pending I/O 
			/// requests.
			/// </summary>
			ExecuteInPersistentThread = WT_EXECUTEINPERSISTENTTHREAD,

			/// <summary>
			/// The callback function can perform a long wait. This flag helps the 
			/// system to decide if it should create a new thread.
			/// </summary>
			ExecuteLongFunction = WT_EXECUTELONGFUNCTION,

			/// <summary>
			/// The timer will be set to the signaled state only once. If this flag 
			/// is set, the Period parameter must be zero.
			/// </summary>
			ExecuteOnlyOnce = WT_EXECUTEONLYONCE,

			/// <summary>
			/// Callback functions will use the current access token, whether it 
			/// is a process or impersonation token. If this flag is not specified, 
			/// callback functions execute only with the process token.
			/// Windows XP/2000:  This flag is not supported until Windows XP with 
			/// SP2 and Windows Server 2003.
			/// </summary>
			TransferImpersonation = WT_TRANSFER_IMPERSONATION
		};

		DEFINE_ENUM_FLAG_OPERATORS(TimerFlags);

		namespace details
		{
			// Forward definition
			static void CALLBACK TimerProc(void*, BOOLEAN);

			class TimerBase
			{
			public:
				static const size_t DefaultDueTime = 0;

			public:
				TimerBase()
					: m_hTimer(nullptr)
				{

				}

				~TimerBase()
				{
					Stop();
				}

				void Start(TimeSpan interval, size_t dueTime = DefaultDueTime, TimerFlags flags = TimerFlags::ExecuteDefault)
				{
					Start(static_cast<size_t>(interval.GetTotalMilliseconds()), dueTime, flags);
				}
				
				void Start(size_t interval, size_t dueTime = DefaultDueTime, TimerFlags flags = TimerFlags::ExecuteDefault)
				{
					if ((flags & TimerFlags::ExecuteOnlyOnce) == TimerFlags::ExecuteOnlyOnce)
					{
						interval = 0;
					}

					BOOL fSuccess = CreateTimerQueueTimer
					(
						&m_hTimer,
						nullptr,
						TimerProc,
						this,
						static_cast<DWORD>(dueTime),
						static_cast<DWORD>(interval),
						static_cast<ULONG>(flags)
					);

					if (!fSuccess)
					{
						auto ec = std::error_code(::GetLastError(), std::system_category());

						throw std::system_error(ec, "CreateTimerQueueTimer() failed");
					}
				}

				void Stop()
				{
					if (m_hTimer)
					{
						BOOL fSuccess = ::DeleteTimerQueueTimer(nullptr, m_hTimer, nullptr);
						_unused(fSuccess);
						assert(fSuccess);

						m_hTimer = nullptr;
					}
				}

				virtual void OnTimer()
				{
					
				}

				size_t getInterval() const
				{
					return m_interval;
				}

				void setInterval(size_t interval)
				{
					// TODO Change timer
					_unused(interval);
				}

				__declspec(property(get = getInterval, put = setInterval)) size_t Interval;

			protected:
				HANDLE m_hTimer;
				size_t m_interval;
				size_t m_dueTime;
			};

			// TimerProc definition
			void CALLBACK TimerProc(void* param, BOOLEAN/* timerCalled*/)
			{
				auto timer = reinterpret_cast<TimerBase*>(param);

				timer->OnTimer();
			};
		}

		template <class T>
		class Timer
			: public details::TimerBase
		{
		public:
			typedef private void (T::*TimedFunction)(void);

			Timer()
				: m_pClass(nullptr)
				, m_pTimedFunction(nullptr)
			{

			}

			void SetTimedEvent(T *pClass, TimedFunction pFunc)
			{
				m_pClass = pClass;
				m_pTimedFunction = pFunc;
			}

		protected:
			void OnTimer()  
			{
				if (m_pClass && m_pTimedFunction)
				{
					(m_pClass->*m_pTimedFunction)();
				}
			}

		private:
			T *m_pClass;
			TimedFunction m_pTimedFunction;
		};
	}
}
