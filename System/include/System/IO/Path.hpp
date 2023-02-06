#pragma once

#pragma comment(lib, "Shlwapi.lib")

#include <string>

namespace System
{
	namespace IO
	{
		class Path
		{
		public:
			static std::wstring Combine(const std::wstring& path1, const std::wstring& path2);
			static std::wstring GetDirectoryName(const std::wstring& path);
			static std::wstring GetTempDirectoryPath();
			static std::wstring GetRandomFileName();
			static std::wstring GetExtension(const std::wstring& path);
			static std::wstring GetFileNameWithoutExtension(const std::wstring& path);
			static std::wstring GetFileName(const std::wstring& path);
			static bool IsRelative(const std::wstring& path);
			static bool IsAbsolute(const std::wstring& path);
			static std::wstring GetRelativePathTo(const std::wstring& workingDirectory, const std::wstring& path);
		};
	}
}
