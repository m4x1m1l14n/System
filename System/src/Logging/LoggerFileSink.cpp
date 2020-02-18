#include <System/Logging/LoggerFileSink.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <System\Application.hpp>
#include <System\TimeSpan.hpp>
#include <System\Text\Encoding.hpp>
#include <System\IO\File.hpp>
#include <System\IO\Path.hpp>
#include <System\IO\Directory.hpp>

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Timers;

namespace System
{
	namespace Logging
	{
		LoggerFileSink::LoggerFileSink(
			const std::wstring& logsFolder/* = std::wstring()*/, 
			const std::wstring& logFileName/* = std::wstring()*/,
			size_t queueSize/* = 20*/
		)
			: m_queueSize(queueSize)
		{
			auto success = true;

			// By default put logs into <EXECUTABLE_PATH>\\logs
			m_logsFolder = logsFolder.empty()
				? Path::Combine(Application::GetFilePath(), L"logs")
				: logsFolder;

			if (!Directory::Exists(m_logsFolder))
			{
				success = Directory::Create(m_logsFolder);
			}

			// TODO Configurable log rotate

			if (success)
			{
				RemoveOverdueLogs();

				const auto& interval = TimeSpan::FromDays(1);
				const auto& now = DateTime::Now();
				const auto& tommorow = now.AddDays(1);
				// 1 hour after midnight
				const auto& dueDate = DateTime(tommorow.getYear(), tommorow.getMonth(), tommorow.getDay(), 1);

				const auto& dueTime = dueDate - now;
				
				m_removeOverdueLogsTimer.SetTimedEvent(this, &LoggerFileSink::OnRemoveOverdueLogsTimer);
				m_removeOverdueLogsTimer.Start(interval, dueTime, TimerFlags::ExecuteInIOThread);

				if (logFileName.empty())
				{
					GenerateLogFileName();	
				
					m_generateLogFileNameTimer.SetTimedEvent(this, &LoggerFileSink::OnGenerateLogFileNameTimer);
					m_generateLogFileNameTimer.Start(
						TimeSpan::FromDays(1),
						dueTime,
						TimerFlags::ExecuteInIOThread
					);
				}
				else
				{
					LockGuard guard(m_logFileNameLock);
					
					m_logFileName = Path::Combine(m_logsFolder, logFileName);
				}
			}
		}

		LoggerFileSink::~LoggerFileSink()
		{

		}

		void LoggerFileSink::Add(const std::deque<Log_ptr>& logs)
		{
			m_queue.insert(m_queue.end(), logs.begin(), logs.end());

			if (m_queue.size() > m_queueSize)
			{
				this->Flush();
			}
		}

		void LoggerFileSink::Flush()
		{
			try
			{
				if (!m_queue.empty())
				{
					__int64 fileSize = 0;

					const auto& filePath = GetLogFileName();
		
					{
						std::ofstream out(Encoding::ToUTF8(filePath), std::ios::out | std::ios::app);

						if (out.is_open())
						{
							for (const auto& log : m_queue)
							{
								out << "[" << log->getTimestamp().ToString("YYYY/MM/dd HH:mm:ss.fff") << "] "
									<< log->getMessage()
									<< " (" << log->getFunction() << ")"
									<< " [" << log->getFile() << ": " << log->getLine() << "]"
									<< "\n";
							}

							fileSize = out.tellp();

							out.close();
						}
					}

					// Erase processed logs
					m_queue.clear();

					// TODO Configurable range
					if (fileSize > 21 * 1024 * 1024)
					{
						File::Truncate(filePath, 19 * 1024 * 1024);
					}
				}
			}
			catch (const std::exception&)
			{
				// TODO Some kind of critical log???
			}
		}

		std::wstring LoggerFileSink::GetLogFileName()
		{
			LockGuard guard(m_logFileNameLock);

			return m_logFileName;
		}

		void LoggerFileSink::RemoveOverdueLogs()
		{
			const auto& validity = TimeSpan::FromDays(7);

			const auto& files = System::IO::Directory::GetFiles(m_logsFolder, L"*.txt");
			if (!files.empty())
			{
				const auto& now = DateTime::Now();

				const auto& format = System::IO::Path::GetFileName(System::Application::GetFileName()) + L"_Logs_%04d%02d%02d_PID_%d.txt";

				for (const auto& file : files)
				{
					int year, month, day, pid;

					int r = swscanf_s(file.c_str(), format.c_str(), &year, &month, &day, &pid);
					if (r >= 3) 
					{
						const auto& fileTime = DateTime(year, month, day);

						if ((fileTime - now) > validity)
						{
							const auto& filePath = System::IO::Path::Combine(m_logsFolder, file);

							try
							{
								File::Delete(filePath);
							}
							catch (const std::exception&)
							{

							}
						}
					}
				}
			}
		}

		void LoggerFileSink::GenerateLogFileName()
		{
			const auto& now = DateTime::Now();

			std::wstringstream ss;

			ss	<< System::IO::Path::GetFileName(System::Application::GetFileName()) << L"_Logs_"
				<< std::setfill(L'0')
				<< std::setw(4) << now.getYear()
				<< std::setw(2) << now.getMonth()
				<< std::setw(2) << now.getDay()
				<< L"_PID_" << ::GetCurrentProcessId() << L".txt";

			const auto& fileName = System::IO::Path::Combine(m_logsFolder, ss.str());

			{
				LockGuard guard(m_logFileNameLock);

				m_logFileName = fileName;
			}
		}

		void LoggerFileSink::OnRemoveOverdueLogsTimer()
		{
			RemoveOverdueLogs();
		}

		void LoggerFileSink::OnGenerateLogFileNameTimer()
		{
			GenerateLogFileName();
		}
	}
}
