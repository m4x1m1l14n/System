#include <System/Logging/Log.hpp>

#include <map>
#include <stdexcept>

namespace System
{
	namespace Logging
	{
		Log::Log()
			: m_line(-1)
		{

		}

		Log::Log(const LogSeverity severity, const std::string & message, const std::string & file, const std::string & function, int line)
			: m_severity(severity)
			, m_message(message)
			, m_file(file)
			, m_function(function)
			, m_line(line)
			, m_timestamp(DateTime::Now())
		{

		}

		DateTime Log::getTimestamp() const
		{
			return m_timestamp;
		}

		LogSeverity Logging::Log::getSeverity() const
		{
			return m_severity;
		}

		std::string Log::getMessage() const
		{
			return m_message;
		}

		std::string Log::getFile() const
		{
			return m_file;
		}

		std::string Log::getFunction() const
		{
			return m_function;
		}

		int Log::getLine() const
		{
			return m_line;
		}

		LogSeverity Log::ParseLogSeverity(const std::string& severity)
		{
			const std::map<std::string, LogSeverity> map =
			{
				{ "emergency",	LogSeverity::Emergency },
				{ "alert",		LogSeverity::Alert },
				{ "critical",	LogSeverity::Critical },
				{ "error",		LogSeverity::Error },
				{ "warning",	LogSeverity::Warning },
				{ "notice",		LogSeverity::Notice },
				{ "info",		LogSeverity::Info },
				{ "debug",		LogSeverity::Debug }
			};

			auto iter = map.find(severity);
			if (iter == map.end())
			{
				throw std::invalid_argument("Invalid argument \"" + severity + "\" provided.");
			}

			return iter->second;
		}
	}
}
