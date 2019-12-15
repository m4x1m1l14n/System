#include <System/Application.hpp>

#include <System/IO/Path.hpp>

#ifdef _WIN32

#include <Windows/Helpers.hpp>

#endif // _WIN32

namespace System
{
	std::wstring Application::GetFileName()
	{
		wchar_t szFileName[MAX_PATH];

		if (::GetModuleFileNameW(HINST_THISCOMPONENT, szFileName, MAX_PATH))
		{
			return std::wstring(szFileName);
		}

		return std::wstring();
	}

	std::wstring Application::GetFilePath()
	{
		const auto& fileName = Application::GetFileName();

		std::wstring ret = System::IO::Path::GetDirectoryName(fileName);

		return ret;
	}
}
