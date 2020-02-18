#pragma once

#include <ppl.h>
#include <ppltasks.h>

#include <queue>

#include <System/Logging/Log.hpp>

namespace System
{
	namespace Logging
	{
		class LoggerSink
		{
			friend class Logger;

		public:
			LoggerSink();
			virtual ~LoggerSink();

		protected:
			// Adds range of logs into sink
			virtual void Add(const std::deque<Log_ptr>& logs) = 0;

			// Force logs to be flushed to IO stream (file, network, etc.) 
			virtual void Flush() = 0;
		};

		typedef std::shared_ptr<LoggerSink> LoggerSink_ptr;
	}
}
