#pragma once

#pragma comment(lib, "Shlwapi.lib")

#include <Windows/Mutex.hpp>
#include <System/Logging/LoggerSink.hpp>
#include <System\Threading\ManualResetEvent.hpp>
#include <System\Threading\AutoResetEvent.hpp>

#include <Shlwapi.h>

#include <sstream>

using namespace System::Threading;

namespace System
{
	namespace Logging
	{
		class Logger
		{
		public:
			Logger();

			virtual ~Logger();

			void Initialize(const std::vector<LoggerSink_ptr>& sinks, LogSeverity severity);

			void AddLog(Log_ptr log);
			bool Pause();
			bool Resume();
			void Stop();
			void Flush();

			bool IsPaused() const;
			bool IsRunning() const;

			/// Setters
			void setSeverity(LogSeverity severity);

			/// Getters
			LogSeverity getSeverity() const;
		
			/// Properties
			__declspec(property(get = getSeverity, put = setSeverity)) LogSeverity Severity;

		protected:
			virtual void WorkerThreadProc();

		protected:
			// NOTE
			//	DO NOT CHANGE BELOW
			//	We are using std::thread, instead of concurrency::task, because when task object
			//	is joined in Logger destructor, exception is thrown. Dont know why, byt imho ThreadPool
			//	is already destroyed, when Logger object dtor is called
			std::thread m_workerThread;

			ManualResetEvent_ptr m_terminateEvent;
			AutoResetEvent_ptr m_queueChangedEvent;

			ManualResetEvent_ptr m_pauseEvent;
			ManualResetEvent_ptr m_resumeEvent;
			bool m_pausedByMe;

			Mutex m_sinksLock;
			std::vector<LoggerSink_ptr> m_sinks;

			Mutex m_queueLock;
			std::deque<Log_ptr> m_queue;

			LogSeverity m_severity;
		};

		/// <summary>
		/// Unique pointer to Logger class instance
		/// </summary>
		typedef std::unique_ptr<Logger> Logger_ptr;

		extern Logger_ptr logger;
	}
}

/// Compile time Logger optimizations
#define COMPILE_TIME_SEVERITY_NONE	   -1
#define COMPILE_TIME_SEVERITY_EMERGENCY	0
#define COMPILE_TIME_SEVERITY_ALERT		1
#define COMPILE_TIME_SEVERITY_CRITICAL	2
#define COMPILE_TIME_SEVERITY_ERROR		3
#define COMPILE_TIME_SEVERITY_WARNING	4
#define COMPILE_TIME_SEVERITY_NOTICE	5
#define COMPILE_TIME_SEVERITY_INFO		6
#define COMPILE_TIME_SEVERITY_DEBUG		7

#define COMPILE_TIME_MAX_SEVERITY COMPILE_TIME_SEVERITY_DEBUG

#define TRACE(sev, text)																		\
do {																							\
	std::ostringstream __ss;																	\
	__ss << text;																				\
																								\
	const auto __log = std::make_shared<Logging::Log>												\
	(																							\
		sev,																					\
		__ss.str(),																				\
		PathFindFileNameA(__FILE__),															\
		__FUNCTION__,																			\
		__LINE__																				\
	);																							\
																								\
	Logging::logger->AddLog(__log);																\
} while (0)

static std::string dump_exception(const std::exception& e)
{
	std::string ret = e.what();

	try
	{
		std::rethrow_if_nested(e);

		return ret;
	}
	catch (const std::exception& ex)
	{
		return ret + "\n" + dump_exception(ex);
	}
}

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_EMERGENCY
#	define TRACE_EMERGENCY(text) TRACE(Logging::LogSeverity::Emergency, "[EMERGENCY] " << text)
#else
#	define TRACE_EMERGENCY(text) ((void)0)
#endif

/// Alert
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_ALERT
#	define TRACE_ALERT(text) TRACE(Logging::LogSeverity::Alert, "[ALERT] " << text)
#else
#	define TRACE_ALERT(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_CRITICAL
#	define TRACE_CRITICAL(text) TRACE(Logging::LogSeverity::Critical, "[CRITICAL] " << text)
#else
#	define TRACE_CRITICAL(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_ERROR
#	define TRACE_ERROR(text) TRACE(Logging::LogSeverity::Error, "[ERROR] " << text)
#else
#	define TRACE_ERROR(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_WARNING
#	define TRACE_WARNING(text) TRACE(Logging::LogSeverity::Warning, "[WARNING] " << text)
#else
#	define TRACE_WARNING(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_NOTICE
#	define TRACE_NOTICE(text) TRACE(Logging::LogSeverity::Notice, "[NOTICE] " << text)
#else
#	define TRACE_NOTICE(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_INFO
#	define TRACE_INFO(text) TRACE(Logging::LogSeverity::Info, text)
#else
#	define TRACE_INFO(text) ((void)0)
#endif

/// Emergency
#if COMPILE_TIME_MAX_SEVERITY >= COMPILE_TIME_SEVERITY_DEBUG
#	define TRACE_DEBUG(text) TRACE(Logging::LogSeverity::Debug, "[DEBUG] " << text)
#else
#	define TRACE_DEBUG(text) ((void)0)
#endif
