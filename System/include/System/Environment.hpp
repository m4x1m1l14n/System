#pragma once

#include <string>

namespace System
{
	enum SpecialFolder
	{
		System,
		WindowsFolder,
		LocalApplicationData,
		LocalTemp,
		ProgramFiles,
		ProgramFilesX86,
		Desktop,
		CommonDesktop,
		CommonPrograms,
		StartMenu
	};

	enum class MachineRole
	{
		StandaloneWorkstation,
		MemberWorkstation,
		StandaloneServer,
		MemberServer,
		BackupDomainController,
		PrimaryDomainController
	};

	class Environment
	{
	public:
		static std::wstring GetFolderPath(SpecialFolder folder);
		static std::wstring GetComputerName();
		// static std::string GetMachineGUID();
		// static std::string GetOperatingSystemVersion();
		static std::wstring GetHostDomainName();
		static std::string GetHostDomainNameA();
		static MachineRole GetMachineRole();
		static bool IsDomainJoined();
	};
}
