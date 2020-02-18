#pragma once

#include <System/Logging/LoggerSink.hpp>

#include <System/Timers/Timer.hpp>
#include <Windows/Mutex.hpp>

namespace System
{
	namespace Logging
	{
		class LoggerFileSink final
			: public LoggerSink
		{
		public:
			LoggerFileSink(
				const std::wstring& logsFolder = std::wstring(), 
				const std::wstring& logFileName = std::wstring(),
				size_t queueSize = 20
			);

			~LoggerFileSink();

			virtual void Add(const std::deque<Log_ptr>& logs) override;
			virtual void Flush() override;

		protected:
			std::wstring GetLogFileName();

			void RemoveOverdueLogs();
			void GenerateLogFileName();

			void OnRemoveOverdueLogsTimer();
			void OnGenerateLogFileNameTimer();

		protected:
			std::deque<Log_ptr> m_queue;
			size_t m_queueSize;
		
			std::wstring m_logsFolder;
			Mutex m_logFileNameLock;
			std::wstring m_logFileName;

		 	System::Timers::Timer<LoggerFileSink> m_removeOverdueLogsTimer;
			System::Timers::Timer<LoggerFileSink> m_generateLogFileNameTimer;
		};
	}
}
