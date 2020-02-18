#pragma once

namespace System
{
	namespace Logging
	{
		/// <summary>
		///
		/// </summary>
		enum class LogSeverity
		{
			Emergency = 0, // system is unusable
			Alert = 1,     // action must be taken immediately
			Critical = 2,  // critical conditions
			Error = 3,     // error conditions
			Warning = 4,   // warning conditions
			Notice = 5,    // normal but significant condition
			Info = 6,      // informational messages
			Debug = 7      // debug-level messages
		};
	}
}
