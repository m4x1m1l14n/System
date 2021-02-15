#pragma once

#include <string>
#include <vector>

namespace System
{
	namespace IO
	{
		class Directory
		{
		public:
			static const std::wstring emptyExtension;

			static bool Exists(const std::wstring& path);
			static bool Empty(const std::wstring& path);
			static bool Delete(const std::wstring& path);
			static bool Copy(const std::wstring& src, const std::wstring& dst);
			static bool Create(const std::wstring &directory);
			static bool DeleteFilesByExtension(const std::wstring& path, const std::wstring& ext);
			static bool IsEmpty(const std::wstring& path);
			static std::wstring GetCurrent();
			static bool SetCurrent(const std::wstring& path);
			static std::vector<std::wstring> GetFiles(const std::wstring& path, const std::wstring& ext = emptyExtension);
		};
	}
}
