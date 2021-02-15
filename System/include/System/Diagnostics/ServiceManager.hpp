#pragma once

#include <Windows.h>

#include <string>

namespace System
{
	namespace Diagnostics
	{
		class ServiceManager
		{
		private:
			ServiceManager();
			~ServiceManager();

		public:
			static bool Install(const std::wstring& serviceName, const std::wstring& displayName, const std::wstring& file, const std::wstring& dependencies = L"");
			static bool Delete(const std::wstring& serviceName);
			static bool IsInstalled(const std::wstring& serviceName);

			static bool Start(const std::wstring& serviceName);
			static bool IsRunning(const std::wstring& serviceName);
			static bool Stop(const std::wstring& serviceName);

			static std::wstring GetServiceImagePath(const std::wstring& ServiceName);

			static bool ConfigureFailActions(const std::wstring& serviceName);
			static bool SetDelayedAutoStart(const std::wstring& serviceName, bool value);

			static DWORD GetServiceProcessId(const std::wstring& serviceName);
			static HANDLE GetServiceProcessHandle(const std::wstring& serviceName);

			static bool DoInstall(const std::wstring& serviceName, const std::wstring& displayName, bool delayedStart);
			static bool DoRestart(const std::wstring& serviceName);
			static bool DoRemove(const std::wstring& serviceName);
		};
	}
}
