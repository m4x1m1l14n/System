#include <System/Application.hpp>

#include <System/IO/Path.hpp>

#ifdef _WIN32
#include <Windows/Helpers.hpp>
#else
#include <unistd.h>
#endif // !_WIN32

namespace System
{
	std::filesystem::path Application::GetFileName()
	{
		return std::filesystem::current_path();

#if defined(_WIN32) && 0
		wchar_t szFileName[MAX_PATH];

		if (::GetModuleFileNameW(HINST_THISCOMPONENT, szFileName, MAX_PATH))
		{
			return std::wstring(szFileName);
		}

		return std::wstring();
#endif // _WIN32
	}

	std::filesystem::path Application::GetFilePath()
	{
		return Application::GetFileName()
			.remove_filename();
		
#if defined(_WIN32) && 0
		const auto& fileName = Application::GetFileName();

		std::wstring ret = System::IO::Path::GetDirectoryName(fileName);

		return ret;
#endif // _WIN32
	}
}
