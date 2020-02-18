///
/// Inspired by
///	https://www.codeproject.com/Articles/146617/Simple-C-Timer-Wrapper
///
#pragma once

#include <assert.h>

#include <System/Timers/TimerFlags.hpp>
#include <System/TimeSpan.hpp>

namespace System
{
	namespace Timers
	{
		DEFINE_ENUM_FLAG_OPERATORS(TimerFlags);

		namespace details
		{
			// Forward definition
			static void CALLBACK TimerProc(void*, BOOLEAN);

			class TimerBase
			{
			public:
				TimerBase()
					: m_hTimer(nullptr)
				{

				}

				~TimerBase()
				{
					try
					{
						Stop();
					}
					catch (...) { }
				}

				void Start(const TimeSpan& interval)
				{
					this->Start(interval, TimeSpan::Zero());
				}

				void Start(const TimeSpan& interval, const TimeSpan& dueTime)
				{
					this->Start(interval, dueTime, TimerFlags::ExecuteDefault);
				}

				void Start(const TimeSpan& interval, const TimeSpan& dueTime, const TimerFlags flags)
				{
					this->Start(
						static_cast<size_t>(interval.GetTotalMilliseconds()),
						static_cast<size_t>(dueTime.GetTotalMilliseconds()), 
						flags
					);
				}
				
				void Start(size_t interval, size_t dueTime, TimerFlags flags)
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
						
						m_hTimer = nullptr;
						
						if (!fSuccess)
						{
							const auto lastError = ::GetLastError();

							throw std::system_error(
								std::error_code(lastError, std::system_category()),
								"DeleteTimerQueueTimer() returned FALSE"
							);
						}
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
