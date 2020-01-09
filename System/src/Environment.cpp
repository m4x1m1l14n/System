#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Wbemuuid.lib")
#pragma comment(lib, "Netapi32.lib")

#include <Windows.h>
#include <ShlObj.h>
#include <LM.h>
#include <DSRole.h>

// #include <Registry.hpp>

#include <System/Environment.hpp>
#include <System/IO/Path.hpp>
#include <System/Text/Encoding.hpp>

// #include <ComPtr.h>

#include <algorithm>
#include <locale>
#include <wbemcli.h>

#include <WbemProv.h>

using namespace System;
using namespace System::IO;

std::wstring Environment::GetFolderPath(SpecialFolder folder)
{
	wchar_t path[MAX_PATH];

	switch (folder)
	{
	case SpecialFolder::LocalApplicationData:
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);

		return path;

	case SpecialFolder::ProgramFilesX86:
		SHGetSpecialFolderPath(
			nullptr,
			path,
			CSIDL_PROGRAM_FILESX86,
			FALSE
			);

		return path;

	case SpecialFolder::ProgramFiles:
		SHGetSpecialFolderPath(nullptr,	path, CSIDL_PROGRAM_FILES,FALSE);

		return path;

	case SpecialFolder::LocalTemp:
		return Path::GetTempDirectoryPath();
		break;

	case SpecialFolder::System:
		GetSystemDirectory(path, MAX_PATH);

		return path;

	case SpecialFolder::WindowsFolder:
		GetWindowsDirectory(path, MAX_PATH);

		return path;

	case SpecialFolder::Desktop:
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOPDIRECTORY, FALSE);

		return path;

	case SpecialFolder::CommonDesktop:
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);

		return path;

	case SpecialFolder::CommonPrograms:
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_PROGRAMS, FALSE);

		return path;

	case SpecialFolder::StartMenu:
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_STARTMENU, FALSE);

		return path;

	default:
		return std::wstring();
		break;
	}
}

std::wstring System::Environment::GetComputerName()
{
	std::wstring computerName;

	wchar_t wszComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwLen = ARRAYSIZE(wszComputerName);

	BOOL fReturn = ::GetComputerName(wszComputerName, &dwLen);
	if (fReturn) {
		computerName = std::wstring(wszComputerName, dwLen);
	}

	return computerName;
}

/*
std::string System::Environment::GetMachineGUID()
{
	try
	{
		auto key = Registry::LocalMachine->Open(L"SOFTWARE\\Microsoft\\Cryptography");

		return System::Text::Encoding::ToUTF8(key->GetString(L"MachineGuid"));
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(
			std::runtime_error("GetMachineGUID() failed")
		);
	}
}
*/

/*
std::string System::Environment::GetOperatingSystemVersion()
{
	std::string osVersion;

	ULONG uReturn = 0;

	ComPtr<IWbemLocator> pWbemLocator;

	HRESULT hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pWbemLocator));

	if (SUCCEEDED(hr))
	{
		ComPtr<IWbemServices> pWbemServices;

		hr = pWbemLocator->ConnectServer(L"root\\cimv2", nullptr, nullptr, nullptr, WBEM_FLAG_CONNECT_USE_MAX_WAIT, nullptr, nullptr, &pWbemServices);

		if (hr == WBEM_S_NO_ERROR)
		{
			ComPtr<IEnumWbemClassObject> pEnumerator;

			hr = pWbemServices->ExecQuery(L"WQL", L"SELECT Caption, Version FROM Win32_OperatingSystem", WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnumerator);

			if (hr == WBEM_S_NO_ERROR)
			{
				ComPtr<IWbemClassObject> pClassObject;

				if ((hr = pEnumerator->Next(WBEM_INFINITE, 1L, &pClassObject, &uReturn)) == S_OK)
				{
					if (uReturn != 0)
					{
						VARIANT variant;
						hr = pClassObject->Get(L"Caption", 0L, &variant, nullptr, nullptr);

						if (SUCCEEDED(hr))
						{
							if (variant.vt == VT_BSTR)
							{
								osVersion = System::Text::Encoding::ToUTF8(variant.bstrVal);
							}

							VariantClear(&variant);
						}
						hr = pClassObject->Get(L"Version", 0L, &variant, nullptr, nullptr);

						if (SUCCEEDED(hr))
						{
							if (variant.vt == VT_BSTR)
							{
								osVersion += " " + System::Text::Encoding::ToUTF8(variant.bstrVal);
							}

							VariantClear(&variant);
						}
					}
				}
			}
		}
	}

	return osVersion;
}
*/

std::wstring System::Environment::GetHostDomainName()
{
	std::wstring wszDomainName;

	DSROLE_PRIMARY_DOMAIN_INFO_BASIC * info;

	if ((DsRoleGetPrimaryDomainInformation(NULL, DsRolePrimaryDomainInfoBasic, (PBYTE *)&info) == ERROR_SUCCESS) && (info != nullptr))
	{
		wszDomainName = std::wstring(info->DomainNameFlat);
		DsRoleFreeMemory(info);
	}
	else
	{
		LPWSTR pwszDomainName = NULL;
		DWORD dwDomainNameLen = 0;

		GetComputerNameEx(ComputerNameDnsDomain, NULL, &dwDomainNameLen);
		if (dwDomainNameLen != 0)
		{
			pwszDomainName = (LPWSTR)malloc(sizeof(WCHAR) * dwDomainNameLen);
			if (pwszDomainName)
			{
				if (GetComputerNameEx(ComputerNameDnsDomain, pwszDomainName, &dwDomainNameLen))
				{
					wszDomainName = std::wstring(pwszDomainName, dwDomainNameLen);
				}

				free(pwszDomainName);
			}
		}
	}

	return wszDomainName;
}

std::string System::Environment::GetHostDomainNameA()
{
	return Text::Encoding::ToUTF8(GetHostDomainName());
}

MachineRole System::Environment::GetMachineRole()
{
	DSROLE_MACHINE_ROLE role = DsRole_RoleStandaloneWorkstation;
	DSROLE_PRIMARY_DOMAIN_INFO_BASIC * info;

	if ((DsRoleGetPrimaryDomainInformation(NULL, DsRolePrimaryDomainInfoBasic, (PBYTE *)&info) == ERROR_SUCCESS) && (info != nullptr))
	{
		role = info->MachineRole;
		DsRoleFreeMemory(info);
	}

	return static_cast<MachineRole>(role);
}

bool System::Environment::IsDomainJoined()
{
	auto joined = false;

	LPWSTR lpNameBuffer = nullptr;

	NETSETUP_JOIN_STATUS joinStatus = NETSETUP_JOIN_STATUS::NetSetupUnknownStatus;

	NET_API_STATUS status = NetGetJoinInformation(nullptr, &lpNameBuffer, &joinStatus);
	if (status == NERR_Success)
	{
		joined = joinStatus == NETSETUP_JOIN_STATUS::NetSetupDomainName;
	}

	if (lpNameBuffer) { NetApiBufferFree(lpNameBuffer); }

	return joined;
}

