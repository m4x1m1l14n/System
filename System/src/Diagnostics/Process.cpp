#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#include <System/Diagnostics/Process.hpp>
#include <System/String.hpp>
#include <System/Threading/EventWaitHandle.hpp>

#include <Windows/Helpers.hpp>

#include <TlHelp32.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <Wtsapi32.h>
#include <Userenv.h>
#include <assert.h>

using namespace System;
using namespace System::Threading;

namespace System
{
	namespace Diagnostics
	{
		class ProcessImpl
			: public Process
		{
		public:
			ProcessImpl()
				: ProcessImpl(static_cast<DWORD>(-1), nullptr, nullptr)
			{

			}

			ProcessImpl(DWORD dwProcessId)
				: m_dwProcessId(dwProcessId)
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
				if (hProcess == nullptr)
				{
					auto lastError = ::GetLastError();

					throw std::system_error(
						std::error_code(lastError, std::system_category()),
						"OpenProcess() for PID " + std::to_string(dwProcessId) + " failed"
					);
				}

				m_hProcess = hProcess;
			}

			ProcessImpl(DWORD dwProcessId, HANDLE hProcess, HANDLE hThread)
				: m_dwProcessId(dwProcessId)
				, m_hProcess(hProcess)
				, m_hThread(hThread)
				, m_dwExitCode(static_cast<DWORD>(-1))
			{

			}

			~ProcessImpl()
			{
				this->Close();
			}

			virtual operator bool() const override
			{
				return (
					m_dwProcessId != -1 &&
					m_hProcess != nullptr &&
					m_hThread != nullptr
					);
			}

			virtual operator HANDLE() override
			{
				return m_hProcess;
			}

			virtual HANDLE getProcessHandle() override
			{
				return m_hProcess;
			}

			virtual HANDLE getThreadHandle() override
			{
				return m_hThread;
			}

			virtual DWORD getProcessId() const override
			{
				return m_dwProcessId;
			}

			virtual DWORD getSessionId() const override
			{
				// TODO Query process session id
				return DWORD();
			}

			virtual DWORD getExitCode() const override
			{
				return m_dwExitCode;
			}

			virtual bool getHasExited() const override
			{
				DWORD dwWaitResult = ::WaitForSingleObject(m_hProcess, 0);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					return true;
				}
				else if (dwWaitResult == WAIT_TIMEOUT)
				{
					return false;
				}
				else
				{
					throw std::system_error(std::error_code(GetLastError(), std::system_category()), "WaitForSingleObject() returned " + std::to_string(dwWaitResult));
				}
			}

			virtual bool getIsRunning() override
			{
				return !getHasExited();
			}

			virtual std::wstring getProcessName() const override
			{
				if (m_processName.empty())
				{
					// TODO Get process name
				}

				return m_processName;
			}

			virtual Process_ptr getParent() const override
			{
				throw std::runtime_error("Not implemeneted");
			}

			virtual void Close() override
			{
				SafeCloseHandle(m_hThread);
				SafeCloseHandle(m_hProcess);

				m_dwProcessId = static_cast<DWORD>(-1);
				m_dwExitCode = static_cast<DWORD>(-1);
			}

			virtual void Kill() override
			{
				assert(m_hProcess != nullptr);

				DWORD dwWaitResult = ::WaitForSingleObject(m_hProcess, 0);
				if (dwWaitResult == WAIT_TIMEOUT)
				{
					BOOL fResult = ::TerminateProcess(m_hProcess, 1);
					if (!fResult)
					{
						// TODO Crypto::Hex::Encode() for process handle to exception
						throw std::system_error(std::error_code(GetLastError(), std::system_category()), "TerminateProcess() returned FALSE");
					}
				}
				else if (dwWaitResult != WAIT_OBJECT_0)
				{
					// TODO Crypto::Hex::Encode() for process handle to exception
					throw std::system_error(std::error_code(GetLastError(), std::system_category()), "WaitForSingleObject() returned " + std::to_string(dwWaitResult));
				}
			}

			virtual bool WaitForExit() override
			{
				return WaitForExit(INFINITE);
			}

			virtual bool WaitForExit(DWORD milliseconds) override
			{
				DWORD dwExitCode = 0;

				BOOL fResult = GetExitCodeProcess(m_hProcess, &dwExitCode);
				if (!fResult)
				{
					throw std::system_error(std::error_code(GetLastError(), std::system_category()), "GetExitCodeProcess() returned FALSE");
				}

				auto success = false;

				if (dwExitCode != STILL_ACTIVE)
				{
					m_dwExitCode = dwExitCode;

					this->Close();

					success = true;
				}
				else
				{
					DWORD dwWaitResult = WaitForSingleObject(m_hProcess, milliseconds);
					if (dwWaitResult == WAIT_OBJECT_0)
					{
						success = true;
					}
					// If wait result is not an object 0 nor a timeout
					// throw exception, because wait failed
					else if (dwWaitResult != WAIT_TIMEOUT)
					{
						auto ec = std::error_code(GetLastError(), std::system_category());

						throw std::system_error(ec, "WaitForSingleObject() with " + std::to_string(milliseconds) + " ms timeout returned " + std::to_string(dwWaitResult));
					}
				}

				return success;
			}

		private:
			bool _IsOpened() const
			{
				return (m_hProcess != nullptr);
			}

			void _Open()
			{
				if (!_IsOpened())
				{
					if (m_dwProcessId == -1)
					{
						
					}
				}
			}

		protected:
			std::wstring m_processName;
			HANDLE m_hProcess;
			HANDLE m_hThread;
			DWORD m_dwProcessId;
			DWORD m_dwExitCode;
		};

		Process::Process()
		{

		}

		Process::~Process()
		{

		}

		Process_ptr Process::GetCurrentProcess()
		{
			DWORD dwProcessId = GetCurrentProcessId();

			return std::make_unique<ProcessImpl>(
				dwProcessId
				);
		}

		Process_ptr Process::Start(const std::wstring & filePath, const std::vector<std::wstring>& arguments)
		{
			std::exception_ptr pex;

			Process_ptr process;

			std::wstring commandLine;

			if (!arguments.empty())
			{
				commandLine = filePath;

				for (auto& argument : arguments)
				{
					commandLine += L" " + argument;
				}
			}

			LPWSTR wszCommandLine = (commandLine.empty()) ? nullptr : StrDupW(commandLine.c_str());

			PROCESS_INFORMATION processInfo = { 0 };
			STARTUPINFO startupInfo = { 0 };

			startupInfo.cb = sizeof(startupInfo);

			BOOL fResult = CreateProcess
			(
				filePath.c_str(),		// application name
				wszCommandLine,			// command line arguments
				nullptr,				// process security attributes
				nullptr,				// thread security attributes
				FALSE,					// do not inherit handles
				CREATE_NEW_CONSOLE,		// creation flags
				nullptr,				// environment
				nullptr,				// current directory
				&startupInfo,			// startup info
				&processInfo			// process informations
			);

			if (fResult)
			{
				process = std::make_unique<ProcessImpl>(
					processInfo.dwProcessId,
					processInfo.hProcess,
					processInfo.hThread
					);
			}
			else
			{
				auto ec = std::error_code(GetLastError(), std::system_category());
				auto ex = std::system_error(ec, "CreateProcess() returned FALSE");

				pex = std::make_exception_ptr(ex);
			}

			if (wszCommandLine)
			{
				::LocalFree(wszCommandLine);
			}

			if (pex)
			{
				std::rethrow_exception(pex);
			}

			return process;
		}

		/*bool Process::IsRunning(const std::wstring& name)
		{
			bool exists = false;

			DWORD dwCurrentProcessId = ::GetCurrentProcessId();

			PROCESSENTRY32W entry;
			entry.dwSize = sizeof(entry);

			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			if (snapshot != INVALID_HANDLE_VALUE)
			{
				if (Process32FirstW(snapshot, &entry))
				{
					do
					{
						if (entry.th32ProcessID != dwCurrentProcessId)
						{
							if (wcslen(entry.szExeFile) == name.length())
							{
								if (StrCmpNW(entry.szExeFile, name.c_str(), static_cast<int>(name.length())) == 0)
								{
									exists = true;

									break;
								}
							}
						}

					} while (Process32NextW(snapshot, &entry));
				}

				CloseHandle(snapshot);
			}

			return exists;
		}

		bool Process::IsRunning(const std::wstring & name, DWORD dwSessionId)
		{
			bool exists = false;

			DWORD dwCurrentProcessId = ::GetCurrentProcessId();

			PROCESSENTRY32W entry;
			entry.dwSize = sizeof(entry);

			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			if (snapshot != INVALID_HANDLE_VALUE)
			{
				if (Process32FirstW(snapshot, &entry))
				{
					do
					{
						if (entry.th32ProcessID != dwCurrentProcessId)
						{
							DWORD dwProcessSessionId = 0;

							if (ProcessIdToSessionId(entry.th32ProcessID, &dwProcessSessionId))
							{
								if (
									dwSessionId == dwProcessSessionId &&
									wcslen(entry.szExeFile) == name.length() &&
									StrCmpNW(entry.szExeFile, name.c_str(), static_cast<int>(name.length())) == 0
									)
								{
									exists = true;

									break;
								}
							}
						}

					} while (Process32NextW(snapshot, &entry));
				}

				CloseHandle(snapshot);
			}

			return exists;
		}*/

		bool Process::Kill(const std::wstring& name)
		{
			BOOL fReturn = FALSE;
			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

			DWORD dwCurrentProcessId = ::GetCurrentProcessId();

			PROCESSENTRY32W pEntry;
			pEntry.dwSize = sizeof(pEntry);

			BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
			while (hRes)
			{
				// ak je tento process, nezabijame
				if (pEntry.th32ProcessID != dwCurrentProcessId)
				{
					if (StrCmpNW(pEntry.szExeFile, name.c_str(), (int)name.length()) == 0)
					{
						HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
						if (hProcess != NULL)
						{
							fReturn = TerminateProcess(hProcess, 1);
							if (fReturn == TRUE)
							{
								DWORD dwExitCode = 0;

								if (GetExitCodeProcess(hProcess, &dwExitCode))
								{
									if (dwExitCode == STILL_ACTIVE)
									{
										DWORD dwWaitResult = WaitForSingleObject(hProcess, 5000);

										fReturn = (dwWaitResult != WAIT_TIMEOUT) ? TRUE : FALSE;
									}
									else
									{
										fReturn = TRUE;
									}
								}
							}

							CloseHandle(hProcess);
						}
						else
						{
							/*DWORD dwLastError = */GetLastError();
							// TODO:
						}
					}
				}

				hRes = Process32NextW(hSnapShot, &pEntry);
			}

			CloseHandle(hSnapShot);

			return (fReturn == TRUE) ? true : false;
		}

		struct HANDLE_DATA
		{
			DWORD dwProcessId;
			HWND hWnd;
		};

		BOOL IsMainWindow(HWND hWnd)
		{
			if ((GetWindow(hWnd, GW_OWNER) == HWND_DESKTOP)/* && IsWindowVisible(hWnd)*/)
				return true;

			return false;
		}

		BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
		{
			HANDLE_DATA *data = reinterpret_cast<HANDLE_DATA*>(lParam);
			DWORD dwProcessId = 0;

			GetWindowThreadProcessId(hWnd, &dwProcessId);

			if ((data->dwProcessId != dwProcessId) || !IsMainWindow(hWnd)) {
				return TRUE;
			}

			data->hWnd = hWnd;

			return FALSE;
		}

		HWND Process::GetMainWindowHandle(DWORD dwProcessId)
		{
			HANDLE_DATA data;
			data.dwProcessId = dwProcessId;
			data.hWnd = nullptr;

			EnumWindows(EnumWindowsCallback, (LPARAM)&data);

			return data.hWnd;
		}

		HANDLE Process::GetParentProcess(DWORD dwAccessRights/* = PROCESS_ALL_ACCESS*/, HANDLE hProcess/* = nullptr*/)
		{
			HANDLE hParent = nullptr;

			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnapshot)
			{
				PROCESSENTRY32 processEntry = { 0 };
				processEntry.dwSize = sizeof(processEntry);

				DWORD dwPID = (hProcess == nullptr) ? GetCurrentProcessId() : GetProcessId(hProcess);

				if (Process32First(hSnapshot, &processEntry))
				{
					do
					{
						if (processEntry.th32ProcessID == dwPID)
						{
							hParent = OpenProcess(dwAccessRights, FALSE, processEntry.th32ParentProcessID);

							break;
						}

					} while (Process32Next(hSnapshot, &processEntry));
				}

				CloseHandle(hSnapshot);
			}

			return hParent;
		}

		DWORD Process::GetParentProcessId()
		{
			DWORD dwProcessId = static_cast<DWORD>(-1);
			HANDLE hParentProcess = GetParentProcess(PROCESS_QUERY_INFORMATION);
			if (hParentProcess != NULL)
			{
				dwProcessId = ::GetProcessId(hParentProcess);
			}

			return dwProcessId;
		}

		bool System::Diagnostics::Process::IsElevated(HANDLE hProcess/* = NULL*/)
		{
			BOOL fRet = FALSE;
			HANDLE hToken = NULL;
			if (OpenProcessToken((hProcess != NULL) ? hProcess : ::GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
				TOKEN_ELEVATION elevation;
				DWORD cbSize = sizeof(elevation);
				if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
					fRet = elevation.TokenIsElevated;
				}
			}
			if (hToken) { CloseHandle(hToken); }

			return fRet ? true : false;
		}

		bool Process::RunAsUserAndWait(const std::wstring & filename, const std::wstring & args, DWORD dwSessionId, HANDLE hTerminate)
		{
			auto success = false;
			HANDLE hPrimaryToken = INVALID_HANDLE_VALUE;

			if (WTSQueryUserToken(dwSessionId, &hPrimaryToken))
			{
				LPVOID pEnvironment = NULL;

				if (CreateEnvironmentBlock(&pEnvironment, hPrimaryToken, FALSE))
				{
					STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
					PROCESS_INFORMATION processInfo = { 0 };
					SECURITY_ATTRIBUTES processAttributes = { 0 };
					SECURITY_ATTRIBUTES threadAttributes = { 0 };

					LPWSTR wszCommandLine = args.empty() ? NULL : StrDupW(args.c_str());

					if (::CreateProcessAsUser(hPrimaryToken, filename.c_str(), wszCommandLine, &processAttributes, &threadAttributes, FALSE, CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, pEnvironment, NULL, &startupInfo, &processInfo))
					{
						HANDLE pHandles[] = { processInfo.hProcess, hTerminate };

						DWORD dwWaitResult = ::WaitForMultipleObjects(ARRAYSIZE(pHandles), pHandles, FALSE, INFINITE);
						if (dwWaitResult == EventWaitHandle::WaitObject<1>())
						{
							TerminateProcess(processInfo.hProcess, 1);
						}

						DWORD dwExitCode = static_cast<DWORD>(-1);

						GetExitCodeProcess(processInfo.hProcess, &dwExitCode);

						success = (dwExitCode == 0);
					}

					if (wszCommandLine) { LocalFree(wszCommandLine); }

					SafeCloseHandle(processInfo.hProcess);
					SafeCloseHandle(processInfo.hThread);
				}

				if (pEnvironment) { DestroyEnvironmentBlock(pEnvironment); }
			}

			SafeCloseHandle(hPrimaryToken);

			return success;
		}

		std::string Process::RunAsUserAndWaitWithOutput(DWORD dwSessionId, const std::wstring & filename, const std::wstring & args, HANDLE hTerminate)
		{
			std::string ret;

			HANDLE hPrimaryToken = INVALID_HANDLE_VALUE;

			BOOL fSuccess = WTSQueryUserToken(dwSessionId, &hPrimaryToken);
			if (fSuccess)
			{
				LPVOID pEnvironment = nullptr;

				fSuccess = CreateEnvironmentBlock(&pEnvironment, hPrimaryToken, FALSE);
				if (fSuccess)
				{
					SECURITY_ATTRIBUTES secAttr = { 0 };
					secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
					secAttr.bInheritHandle = TRUE;
					secAttr.lpSecurityDescriptor = nullptr;

					HANDLE hChildStdOutWrite = nullptr;
					HANDLE hChildStdOutRead = nullptr;

					fSuccess = CreatePipe(&hChildStdOutRead, &hChildStdOutWrite, &secAttr, 0);
					if (fSuccess)
					{
						fSuccess = SetHandleInformation(hChildStdOutRead, HANDLE_FLAG_INHERIT, 0);
						if (fSuccess)
						{
							HANDLE hChildStdInWrite = nullptr;
							HANDLE hChildStdInRead = nullptr;

							fSuccess = CreatePipe(&hChildStdInRead, &hChildStdInWrite, &secAttr, 0);
							if (fSuccess)
							{
								fSuccess = SetHandleInformation(hChildStdInWrite, HANDLE_FLAG_INHERIT, 0);
								if (fSuccess)
								{
									STARTUPINFO startupInfo = { 0 };

									startupInfo.cb = sizeof(STARTUPINFO);
									startupInfo.hStdError = hChildStdOutWrite;
									startupInfo.hStdOutput = hChildStdOutWrite;
									startupInfo.hStdInput = hChildStdInRead;
									startupInfo.dwFlags |= STARTF_USESTDHANDLES;

									PROCESS_INFORMATION processInfo = { 0 };

									LPSECURITY_ATTRIBUTES pProcessAttributes = nullptr;
									LPSECURITY_ATTRIBUTES pThreadAttributes = nullptr;

									DWORD dwCreationFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW;

									LPWSTR wszCommandLine = args.empty() ? nullptr : StrDupW(args.c_str());

									if (::CreateProcessAsUser(hPrimaryToken, filename.c_str(), wszCommandLine, pProcessAttributes, pThreadAttributes, TRUE, dwCreationFlags, pEnvironment, nullptr, &startupInfo, &processInfo))
									{
										HANDLE pHandles[] = { processInfo.hProcess, hTerminate };

										DWORD dwWaitResult = ::WaitForMultipleObjects(ARRAYSIZE(pHandles), pHandles, FALSE, INFINITE);

										if (dwWaitResult == WAIT_OBJECT_0)
										{
											CHAR pBuffer[2048];
											DWORD dwBytesRead = 0;

											do
											{
												fSuccess = ReadFile(hChildStdOutRead, pBuffer, ARRAYSIZE(pBuffer), &dwBytesRead, nullptr);
												if (fSuccess)
												{
													ret.append(pBuffer, dwBytesRead);
												}
												/*else
												{
													DWORD dwLastError = ::GetLastError();

													int a = 5;
												}*/

											} while (fSuccess && dwBytesRead == ARRAYSIZE(pBuffer));
										}
										else
										{
											TerminateProcess(processInfo.hProcess, 1);
										}
									}

									if (wszCommandLine) { LocalFree(wszCommandLine); }

									SafeCloseHandle(processInfo.hProcess);
									SafeCloseHandle(processInfo.hThread);
								}
							}

							SafeCloseHandle(hChildStdInWrite);
							SafeCloseHandle(hChildStdInRead);
						}
					}

					SafeCloseHandle(hChildStdOutWrite);
					SafeCloseHandle(hChildStdOutRead);
				}

				if (pEnvironment) { DestroyEnvironmentBlock(pEnvironment); }
			}

			SafeCloseHandle(hPrimaryToken);

			return ret;
		}

		std::wstring Process::GetFilePath(const std::wstring& processName)
		{
			std::wstring filePath;
			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

			PROCESSENTRY32W pEntry;
			pEntry.dwSize = sizeof(pEntry);

			BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
			while (hRes)
			{
				if (StrCmpNW(pEntry.szExeFile, processName.c_str(), (int)processName.length()) == 0)
				{
					DWORD dwDesiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;

					HANDLE hProcess = OpenProcess(dwDesiredAccess, 0, (DWORD)pEntry.th32ProcessID);
					if (hProcess != NULL)
					{
						wchar_t szBuf[MAX_PATH];

						if (GetModuleFileNameExW(hProcess, NULL, szBuf, MAX_PATH) > 0)
						{
							filePath = szBuf;
						}

						CloseHandle(hProcess);
					}
					else
					{
						/*DWORD dwLastError = GetLastError();*/
						// TODO:
					}

					break;
				}

				hRes = Process32NextW(hSnapShot, &pEntry);
			}

			CloseHandle(hSnapShot);

			return filePath;
		}

		Process_ptr Process::GetProcessByName(const std::wstring& processName)
		{
			std::exception_ptr pex;

			Process_ptr process;

			auto hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap)
			{
				PROCESSENTRY32 pe;

				pe.dwSize = sizeof(PROCESSENTRY32);

				if (Process32First(hProcessSnap, &pe))
				{
					BOOL fSuccess = FALSE;

					do
					{
						if (String::CompareCaseInsensitive(pe.szExeFile, processName))
						{
							process = std::make_shared<ProcessImpl>(pe.th32ProcessID);

							break;
						}

						fSuccess = Process32Next(hProcessSnap, &pe);
						if (!fSuccess)
						{
							auto lastError = ::GetLastError();
							if (lastError != ERROR_NO_MORE_FILES)
							{
								pex = std::make_exception_ptr(
									std::system_error(
										std::error_code(lastError, std::system_category()),
										"Process32Next() failed"
									)
								);
							}
						}

					} while (fSuccess);
				}
				else
				{
					auto lastError = ::GetLastError();

					if (lastError != ERROR_NO_MORE_FILES)
					{
						pex = std::make_exception_ptr(
							std::system_error(
								std::error_code(lastError, std::system_category()),
								"Process32First() failed"
							)
						);
					}
				}

				::CloseHandle(hProcessSnap);
			}
			else
			{
				auto lastError = ::GetLastError();

				pex = std::make_exception_ptr(
					std::system_error(
						std::error_code(lastError, std::system_category()),
						"CreateToolhelp32Snapshot() failed"
					)
				);
			}

			if (pex)
			{
				std::rethrow_exception(pex);
			}

			return process;
		}

		Process_ptr Process::GetProcessById(DWORD processId)
		{
			std::exception_ptr pex;

			Process_ptr process;

			auto hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap)
			{
				PROCESSENTRY32 pe;

				pe.dwSize = sizeof(PROCESSENTRY32);

				if (Process32First(hProcessSnap, &pe))
				{
					BOOL fSuccess = FALSE;

					do
					{
						if (pe.th32ProcessID == processId)
						{
							process = std::make_shared<ProcessImpl>(pe.th32ProcessID);

							break;
						}

						fSuccess = Process32Next(hProcessSnap, &pe);
						if (!fSuccess)
						{
							auto lastError = ::GetLastError();
							if (lastError != ERROR_NO_MORE_FILES)
							{
								pex = std::make_exception_ptr(
									std::system_error(
										std::error_code(lastError, std::system_category()),
										"Process32Next() failed"
									)
								);
							}
						}

					} while (fSuccess);
				}
				else
				{
					auto lastError = ::GetLastError();

					if (lastError != ERROR_NO_MORE_FILES)
					{
						pex = std::make_exception_ptr(
							std::system_error(
								std::error_code(lastError, std::system_category()),
								"Process32First() failed"
							)
						);
					}
				}

				::CloseHandle(hProcessSnap);
			}
			else
			{
				auto lastError = ::GetLastError();

				pex = std::make_exception_ptr(
					std::system_error(
						std::error_code(lastError, std::system_category()),
						"CreateToolhelp32Snapshot() failed"
					)
				);
			}

			if (pex)
			{
				std::rethrow_exception(pex);
			}

			return process;
		}

		std::vector<Process_ptr> Process::GetProcessesByName(const std::wstring & processName)
		{
			std::exception_ptr pex;

			std::vector<Process_ptr> processes;

			auto hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap)
			{
				PROCESSENTRY32 pe;

				pe.dwSize = sizeof(PROCESSENTRY32);

				if (Process32First(hProcessSnap, &pe))
				{
					BOOL fSuccess = FALSE;

					do
					{
						if (String::CompareCaseInsensitive(pe.szExeFile, processName))
						{
							Process_ptr process = std::make_shared<ProcessImpl>(pe.th32ProcessID);

							processes.push_back(process);
						}

						fSuccess = Process32Next(hProcessSnap, &pe);
						if (!fSuccess)
						{
							auto lastError = ::GetLastError();
							if (lastError != ERROR_NO_MORE_FILES)
							{
								pex = std::make_exception_ptr(
									std::system_error(
										std::error_code(lastError, std::system_category()),
										"Process32Next() failed"
									)
								);
							}
						}

					} while (fSuccess);
				}
				else
				{
					auto lastError = ::GetLastError();

					if (lastError != ERROR_NO_MORE_FILES)
					{
						pex = std::make_exception_ptr(
							std::system_error(
								std::error_code(lastError, std::system_category()),
								"Process32First() failed"
							)
						);
					}
				}

				::CloseHandle(hProcessSnap);
			}
			else
			{
				auto lastError = ::GetLastError();

				pex = std::make_exception_ptr(
					std::system_error(
						std::error_code(lastError, std::system_category()),
						"CreateToolhelp32Snapshot() failed"
					)
				);
			}

			if (pex)
			{
				std::rethrow_exception(pex);
			}

			return processes;
		}
	}
}
