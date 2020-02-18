#include <System/Logging/Logger.hpp>

#include <assert.h>

namespace System
{
	namespace Logging
	{
		Logger::Logger()
			: m_severity(LogSeverity::Info)
			, m_pausedByMe(false)
		{
			m_terminateEvent = std::make_shared<ManualResetEvent>();
			m_queueChangedEvent = std::make_shared<AutoResetEvent>();

			m_pauseEvent = std::make_shared<ManualResetEvent>(false, L"Global\\LoggerPauseEvent{6A4EC85C-04D3-4551-9574-6DFC212D1B19}");
			m_resumeEvent = std::make_shared<ManualResetEvent>(false, L"Global\\LoggerResumeEvent{6A4EC85C-04D3-4551-9574-6DFC212D1B19}");
		}

		Logger::~Logger()
		{
			this->Stop();
		}

		void Logger::Initialize(const std::vector<LoggerSink_ptr>& sinks, LogSeverity severity)
		{
			this->Stop();

			m_severity = severity;

			{
				LockGuard guard(m_sinksLock);

				m_sinks = sinks;
			}

			m_terminateEvent->Reset();

			m_workerThread = std::thread(&Logger::WorkerThreadProc, this);
		}

		void Logger::AddLog(Log_ptr log)
		{
			{
				LockGuard guard(m_queueLock);

				m_queue.push_back(log);

				while (m_queue.size() > 200)
				{
					m_queue.pop_front();
				}
			}

			m_queueChangedEvent->Set();
		}

		bool Logger::Pause()
		{
			// TODO Maybe global lock for pause/resume ops?

			if (!m_pauseEvent->IsSet())
			{
				m_resumeEvent->Reset();
				m_pauseEvent->Set();

				m_pausedByMe = true;

				return true;
			}

			return false;
		}

		bool Logger::Resume()
		{
			// TODO Maybe global lock for pause/resume ops?

			if (m_pauseEvent->IsSet() && m_pausedByMe)
			{
				m_pauseEvent->Reset();
				m_resumeEvent->Set();

				m_pausedByMe = false;

				return true;
			}

			return false;
		}

		void Logger::Stop()
		{
			m_terminateEvent->Set();

			if (m_workerThread.joinable())
			{
				m_workerThread.join();
			}
		}

		void Logger::Flush()
		{
			{
				LockGuard guard(m_sinksLock);

				std::for_each(m_sinks.begin(), m_sinks.end(), [](LoggerSink_ptr pSink)
				{
					pSink->Flush();
				});
			}
		}

		bool Logger::IsPaused() const
		{
			return m_pauseEvent->IsSet();
		}

		bool Logger::IsRunning() const
		{
			return m_workerThread.joinable();
		}

		void Logger::setSeverity(LogSeverity severity)
		{
			m_severity = severity;
		}

		LogSeverity Logger::getSeverity() const
		{
			return m_severity;
		}

		void Logger::WorkerThreadProc()
		{
			int waitResult = EventWaitHandle::WaitTimeout;

			std::vector<EventWaitHandle_ptr> events =
			{ 
				m_terminateEvent,
				m_queueChangedEvent,
				m_pauseEvent
			};

			do
			{
				waitResult = EventWaitHandle::WaitAny(events);

				auto queue = [&]
				{
					LockGuard guard(m_queueLock);

					auto queue = std::move(m_queue);

					m_queue = std::deque<Log_ptr>();
				
					return queue;
				}();

				// Remove logs with lower severity
				for (auto iter = queue.begin(); iter != queue.end(); /* DO NOT INCREMENT*/)
				{
					if ((*iter)->getSeverity() > m_severity)
					{
						iter = queue.erase(iter);
					}
					else
					{
						++iter;
					}
				}

				// Add logs to sinks if some
				if (!queue.empty())
				{
					std::for_each(m_sinks.begin(), m_sinks.end(), [&](LoggerSink_ptr pSink)
					{
						try
						{
							pSink->Add(queue);
						}
						catch (...) { }
					});
				}

				// Pause requested
				if (waitResult == EventWaitHandle::WaitObject<2>())
				{
					// Flush all pending logs
					this->Flush();

					// Wait for resume or terminate event
					waitResult = EventWaitHandle::WaitAny({ m_terminateEvent, m_resumeEvent });
				}

			} while (waitResult == EventWaitHandle::WaitObject<1>());

			// Flush all sinks before exiting worker
			this->Flush();
		}
	}
}
