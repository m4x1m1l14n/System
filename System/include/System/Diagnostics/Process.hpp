#pragma once

#include <Windows.h>

#include <memory>
#include <vector>

// TODO Comments

namespace System
{
	namespace Diagnostics
	{
		class Process;

		typedef std::shared_ptr<Process> Process_ptr;

		class Process
		{
		protected:
			Process();

		public:
			// Disable object copy
			Process(const Process& other) = delete;
			Process& operator=(const Process& other) = delete;

			// Disable object move
			Process(Process&& other) = delete;
			Process& operator=(Process&& other) = delete;

			virtual ~Process();

			virtual operator bool() const = 0;
			virtual operator HANDLE() = 0;

			virtual void Close() = 0;
			virtual void Kill() = 0;
			virtual bool WaitForExit() = 0;
			virtual bool WaitForExit(DWORD milliseconds) = 0;

			virtual HANDLE getProcessHandle() = 0;
			virtual HANDLE getThreadHandle() = 0;
			virtual bool getIsRunning() = 0;
			virtual DWORD getProcessId() const = 0;
			virtual DWORD getExitCode() const = 0;
			virtual bool getHasExited() const = 0;
			virtual std::wstring getProcessName() const = 0;
			virtual DWORD getSessionId() const = 0;
			virtual Process_ptr getParent() const = 0;

			__declspec(property(get = getProcessId)) DWORD Id;
			__declspec(property(get = getIsRunning)) bool IsRunning;
			__declspec(property(get = getExitCode)) DWORD ExitCode;
			__declspec(property(get = getHasExited)) bool HasExited;
			__declspec(property(get = getProcessName)) std::wstring ProcessName;
			__declspec(property(get = getSessionId)) DWORD SessionId;
			__declspec(property(get = getParent)) Process_ptr Parent;
			__declspec(property(get = getProcessHandle)) HANDLE Handle;

			static Process_ptr GetCurrentProcess();
			static Process_ptr Start(const std::wstring& filePath, const std::vector<std::wstring>& arguments = {});

			static Process_ptr GetProcessByName(const std::wstring& processName);
			static Process_ptr GetProcessById(DWORD processId);
			static std::vector<Process_ptr> GetProcessesByName(const std::wstring& processName);

			// TODO Try to replace & remove this obsolete methods

			/*static bool IsRunning(const std::wstring& name);
			static bool IsRunning(const std::wstring& name, DWORD dwSessionId);*/
			static bool Kill(const std::wstring& name);
			static std::wstring GetFilePath(const std::wstring& processName);
			static HWND GetMainWindowHandle(DWORD dwProcessId);
			static HANDLE GetParentProcess(DWORD dwAccessRights = PROCESS_ALL_ACCESS, HANDLE hProcess = nullptr);
			static DWORD GetParentProcessId();
			static bool IsElevated(HANDLE hProcess = NULL);
			static bool RunAsUserAndWait(const std::wstring& filename, const std::wstring& args, DWORD dwSessionId, HANDLE hTerminate);
			static std::string RunAsUserAndWaitWithOutput(DWORD dwSessionId, const std::wstring& filename, const std::wstring& args, HANDLE hTerminate);
		};
	}
}
