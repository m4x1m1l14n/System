#include <System/Application.hpp>

#ifdef _WIN32
#include <Windows/Helpers.hpp>
#else
#include <unistd.h>
#endif // !_WIN32

namespace System
{
	/**
	* Gets currently running executables filename with absolute path.
	*
	* @note std::filesystem::current_path() is not working correctly. Application running as windows service resolves current path to System32 directory!
	*/
	std::filesystem::path Application::GetFileName()
	{
#if defined(_WIN32)

		static std::filesystem::path fileName;

		if (fileName.empty())
		{
			size_t bufferLen = MAX_PATH;
			auto buffer = std::make_unique<wchar_t[]>(bufferLen);

			do
			{
				const auto result = ::GetModuleFileNameW(HINST_THISCOMPONENT, buffer.get(), static_cast<DWORD>(bufferLen));
				if (result == 0)
				{
					const auto lastError = ::GetLastError();

					throw std::system_error(
						std::error_code(lastError, std::system_category()),
						"GetModuleFileNameW() returned 0"
					);
				}
				else if (result == bufferLen)
				{
					const auto lastError = ::GetLastError();

					if (lastError == ERROR_INSUFFICIENT_BUFFER)
					{
						bufferLen *= 2;
						buffer = std::make_unique<wchar_t[]>(bufferLen);
					}
					else
					{
						throw std::system_error(
							std::error_code(lastError, std::system_category()),
							"GetModuleFileNameW() returned " + std::to_string(result)
						);
					}
				}
				else
				{
					fileName = std::filesystem::path(buffer.get());
				}

			} while (fileName.empty());
		}

		return fileName;

#else
		static_assert(false, "Implement");
#endif
	}

	std::filesystem::path Application::GetFilePath()
	{
		static std::filesystem::path applicationFilePath;

		if (applicationFilePath.empty())
		{
			applicationFilePath = Application::GetFileName()
				.remove_filename();
		}

		return applicationFilePath;
		
#if defined(_WIN32) && 0
		const auto& fileName = Application::GetFileName();

		std::wstring ret = System::IO::Path::GetDirectoryName(fileName);

		return ret;
#endif // _WIN32
	}
}
