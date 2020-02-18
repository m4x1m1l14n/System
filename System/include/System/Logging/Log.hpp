#pragma once

#include <string>
#include <memory>

#include <System/Logging/LogSeverity.hpp>
#include <System/DateTime.hpp>

namespace System
{
	namespace Logging
	{
		class Log
		{
		public:
			Log();
			Log(const LogSeverity severity, const std::string& message, const std::string& file, const std::string& function, int line);

			// NOTE:
			//	Do not implement destructor, so compiler can create default move logic
			// virtual ~Log();

			DateTime getTimestamp() const;
			LogSeverity getSeverity() const;
			std::string getMessage() const;
			std::string getFile() const;
			std::string getFunction() const;
			int getLine() const;

#if defined(_WIN32) && 0
			__declspec(property(get = getTimestamp)) FILETIME Timestamp;
			__declspec(property(get = getSeverity)) LogSeverity Severity;
			__declspec(property(get = getMessage)) std::string Message;
			__declspec(property(get = getFile)) std::string File;
			__declspec(property(get = getFunction)) std::string Function;
			__declspec(property(get = getLine)) int Line;
#endif

			/// <summary>
			///
			/// </summary>
			/// <exception cref="">Throws std::invalid_argument</exception>
			static LogSeverity ParseLogSeverity(const std::string& severity);

		protected:
			DateTime m_timestamp;

			LogSeverity m_severity;
			std::string m_message;
			std::string m_file;
			std::string m_function;
			int m_line;
		};

		typedef std::shared_ptr<Log> Log_ptr;
	}
}
