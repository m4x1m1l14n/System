#include <System/IO/Path.hpp>

#include <Windows.h>
#include <Shlwapi.h>

namespace System
{
	namespace IO
	{
		std::wstring Path::Combine(const std::wstring& path1, const std::wstring& path2)
		{
			wchar_t szBuf[MAX_PATH];

			memcpy(szBuf, path1.c_str(), (path1.length() + 1) * sizeof(wchar_t));

			if (PathAppendW(szBuf, path2.c_str()))
			{
				return szBuf;
			}

			return std::wstring();

#if 0
			std::wstring a, b;
			size_t pos;

			pos = path1.find_last_of(L'\\');
			if (pos == (path1.length() - 1))
			{
				a = path1.substr(0, pos);
			}
			else
			{
				a = path1;
			}

			pos = path2.find_first_of(L'\\');
			if (pos == 0)
			{
				b = path2.substr(1);
			}
			else
			{
				b = path2;
			}

			return a + L'\\' + b;
#endif
		}

		std::wstring Path::GetDirectoryName(const std::wstring fileName)
		{
			size_t pos = fileName.find_last_of(L"/\\");
			if (pos != std::wstring::npos)
			{
				if (fileName.length() == pos)
					return fileName;
				else
					return fileName.substr(0, pos + 1);
			}

			return L"";
		}

		std::wstring Path::GetExtension(const std::wstring& path)
		{
			return std::wstring(
				PathFindExtension(path.c_str())
			);
		}

		std::wstring Path::GetRandomFileName()
		{
			// TODO implement
			return std::wstring();
		}

		std::wstring Path::GetTempDirectoryPath()
		{
			WCHAR tempPath[MAX_PATH];

			DWORD dwRet = ::GetTempPathW(MAX_PATH, tempPath);
			if (dwRet > 0) {
				return std::wstring(tempPath, dwRet);
			}

			return std::wstring();
		}

		std::wstring Path::GetFileNameWithoutExtension(const std::wstring& path)
		{
			std::wstring ret;
			auto len = path.length() + 1;
			WCHAR *wszPath = new WCHAR[len];
			if (wszPath) {
				memcpy(wszPath, path.c_str(), len * sizeof(WCHAR));
				PathRemoveExtension(wszPath);
				auto fileName = PathFindFileName(wszPath);
				if (fileName != wszPath) {
					ret = fileName;
				}

				delete[] wszPath;
			}
			
			return ret;
		}

		std::wstring Path::GetFileName(const std::wstring & path)
		{
#if 0
			std::wstring ret;
			
			if (!path.empty()) {
				auto len = path.length() + 1;
				WCHAR *wszPath = new WCHAR[len];
				if (wszPath) {
					memcpy(wszPath, path.c_str(), len * sizeof(WCHAR));
					auto fileName = PathFindFileName(wszPath);
					if (fileName != wszPath) {
						ret = fileName;
					}

					delete[] wszPath;
				}
			}
			return ret;
#else
			std::wstring ret;

			if (!path.empty()) {
				size_t pos = path.find_last_of(L"\\/");
				if (pos != std::wstring::npos) {
					ret = path.substr(pos + 1);
				}
			}

			return ret;
#endif
		}

		bool Path::IsRelative(const std::wstring & path)
		{
			return (::PathIsRelative(path.c_str()) == TRUE) ? true : false;
		}

		bool Path::IsAbsolute(const std::wstring & path)
		{
			return !Path::IsRelative(path);
		}

		std::wstring Path::GetRelativePathTo(const std::wstring& workingDirectory, const std::wstring& path)
		{
			wchar_t relativePath[MAX_PATH];

			BOOL fResult = PathRelativePathTo(relativePath, workingDirectory.c_str(), FILE_ATTRIBUTE_DIRECTORY, path.c_str(), FILE_ATTRIBUTE_NORMAL);
			if (fResult != FALSE)
			{
				return relativePath;
			}

			return std::wstring();
		}
	}
}
