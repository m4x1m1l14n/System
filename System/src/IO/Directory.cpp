#include <System/IO/Directory.hpp>

#include <System/IO/Path.hpp>
#include <System/Application.hpp>

#include <Windows.h>
#include <Shlwapi.h>

namespace System
{
	namespace IO
	{
		const std::wstring Directory::emptyExtension = std::wstring();

		bool Directory::Exists(const std::wstring& path)
		{
			DWORD dwAttrib = GetFileAttributes(path.c_str());
			if (dwAttrib != INVALID_FILE_ATTRIBUTES)
			{
				return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
			}

			return false;
		}

		bool Directory::Empty(const std::wstring& path)
		{
			BOOL fReturn = TRUE;
			auto filter = Path::Combine(path, L"\\*.*");

			WIN32_FIND_DATA findData;

			HANDLE hFile = FindFirstFile(filter.c_str(), &findData);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (!((lstrcmp(findData.cFileName, L".") == 0) || (lstrcmp(findData.cFileName, L"..") == 0)))
					{
						auto subFolder = Path::Combine(path, findData.cFileName);

						if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
						{
							fReturn = Empty(subFolder);
							if (fReturn == TRUE)
							{
								fReturn = RemoveDirectory(subFolder.c_str());
								if (fReturn == FALSE)
								{
									DWORD dwLastError = GetLastError();
									if (dwLastError != ERROR_FILE_NOT_FOUND)
									{
										auto currentDirectory = Directory::GetCurrent();
										if (subFolder == currentDirectory)
										{
											Directory::SetCurrent(Application::GetFilePath());

											fReturn = RemoveDirectory(subFolder.c_str());
										}
									}
									else
									{
										fReturn = TRUE;
									}
								}
							}
						}
						else
						{
							fReturn = DeleteFile(subFolder.c_str());
						}
					}
				} while (fReturn && FindNextFile(hFile, &findData));

				FindClose(hFile);
			}

			return (fReturn == TRUE) ? true : false;
		}

		std::wstring Directory::GetCurrent()
		{
			std::wstring path;
			wchar_t buff[MAX_PATH];

			if (::GetCurrentDirectory(MAX_PATH, buff) > 0)
			{
				path = buff;

				size_t pos = path.find_last_of(L"/\\");
				if (pos != (path.length() - 1))
				{
					path += L"\\";
				}
			}

			return path;
		}

		bool Directory::SetCurrent(const std::wstring& path)
		{
			return (::SetCurrentDirectory(path.c_str()) == TRUE) ? true : false;
		}

		std::vector<std::wstring> Directory::GetFiles(const std::wstring& path, const std::wstring& ext/* = emptyExtension*/)
		{
			std::vector<std::wstring> files;

			auto filter = ext.empty() ? Path::Combine(path, L"*.*") : Path::Combine(path, ext);

			WIN32_FIND_DATA findData = {};

			HANDLE hFind = FindFirstFile(filter.c_str(), &findData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
					{
						if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
						{
							files.push_back(findData.cFileName);
						}
					}
				} while (FindNextFile(hFind, &findData));

				FindClose(hFind);
			}

			return files;
		}

		bool Directory::Delete(const std::wstring& path)
		{
			Directory::Empty(path);

			DWORD dwLastError = ERROR_SUCCESS;

			BOOL fReturn = RemoveDirectory(path.c_str());
			if ((fReturn == FALSE))
			{
				dwLastError = GetLastError();
				if (dwLastError != ERROR_FILE_NOT_FOUND)
				{
					auto currentDirectory = Directory::GetCurrent();
					if (path == currentDirectory)
					{
						Directory::SetCurrent(Application::GetFilePath());

						fReturn = RemoveDirectory(path.c_str());
					}
				}
				else
				{
					fReturn = TRUE;
				}
			}

			return (fReturn == TRUE) ? true : false;
		}

		bool Directory::Copy(const std::wstring& src, const std::wstring& dst)
		{
			BOOL fReturn = TRUE;
			auto filter = Path::Combine(src, L"*.*");

			WIN32_FIND_DATA findData;

			HANDLE hFile = FindFirstFile(filter.c_str(), &findData);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (!((lstrcmp(findData.cFileName, L".") == 0) || (lstrcmp(findData.cFileName, L"..") == 0)))
					{
						auto subFolder = Path::Combine(src, findData.cFileName);
						auto dstSubFolder = Path::Combine(dst, findData.cFileName);

						if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
						{
							fReturn = CreateDirectory(dstSubFolder.c_str(), NULL);
							if (fReturn == FALSE)
							{
								DWORD dwLastError = GetLastError();
								if (dwLastError == ERROR_ALREADY_EXISTS)
								{
									fReturn = TRUE;
								}
							}

							if (fReturn == TRUE)
							{
								fReturn = Copy(subFolder, dstSubFolder);
							}
						}
						else
						{
							fReturn = CopyFile(subFolder.c_str(), dstSubFolder.c_str(), FALSE);
						}
					}
				} while (fReturn && FindNextFile(hFile, &findData));

				FindClose(hFile);
			}

			return (fReturn == TRUE) ? true : false;
		}

		bool Directory::Create(const std::wstring &directory)
		{
			if (directory.empty()) { return false; }

			auto path = directory;

			if (path.find_last_of(L'\\') != path.length()) {
				path += L"\\";
			}

			auto success = true;
			size_t pos = 0;
			while ((pos = path.find_first_of(L'\\', pos)) != std::wstring::npos) {
				auto subDirectory = path.substr(0, ++pos);
				if (!Directory::Exists(subDirectory)) {
					if (!CreateDirectory(subDirectory.c_str(), NULL)) {
						success = false;
						break;
					}
				}
			}

			return success;

			/*BOOL fReturn = TRUE;

			size_t len = directory.length() + 1;
			WCHAR *wszPath = nullptr;
			auto c = directory[directory.length()];
			if (c != L'\\' || c != L'/') {
				wszPath = new WCHAR[len + 1];
				if (wszPath) {
					memcpy(wszPath, directory.c_str(), (len + 1) * sizeof(WCHAR));
					wszPath[len] = L'\\';
					wszPath[++len] = L'\0';
				}
			}
			else {
				wszPath = new WCHAR[len];
				if (wszPath) {
					memcpy(wszPath, directory.c_str(), len * sizeof(WCHAR));
				}
			}

			if (wszPath == nullptr) {
				return false;
			}

			auto dir = directory;
			wchar_t szDirectoryName[MAX_PATH];
			WCHAR* p = wszPath;
			wchar_t* q = szDirectoryName;

			while ((*p != L'\0') && (fReturn != FALSE))
			{
				if ((L'\\' == *p) || (L'/' == *p))
				{
					if (L':' != *(p - 1))
					{
						BOOL fReturn = CreateDirectory(szDirectoryName, NULL);
						if (fReturn == FALSE)
						{
							DWORD dwLastError = GetLastError();
							if (dwLastError != ERROR_ALREADY_EXISTS)
							{
								fReturn = FALSE;
							}
						}
					}
				}

				*q++ = *p++;
				*q = '\0';
			}

			delete[] wszPath;

			return (fReturn == TRUE) ? true : false;*/
		}

		bool Directory::DeleteFilesByExtension(const std::wstring& path, const std::wstring& ext)
		{
			wchar_t szFilter[MAX_PATH];

			memcpy(szFilter, path.c_str(), (path.length() + 1) * sizeof(char));

			BOOL fReturn = PathAddExtension(szFilter, ext.c_str());
			if (fReturn == TRUE)
			{
				WIN32_FIND_DATA findData;

				HANDLE hFile = FindFirstFile(szFilter, &findData);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					wchar_t szBuf[MAX_PATH];

					do
					{
						if (!((lstrcmp(findData.cFileName, L".") == 0) || (lstrcmp(findData.cFileName, L"..") == 0)))
						{
							if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
							{
								if (PathCombine(szBuf, path.c_str(), findData.cFileName) != NULL)
								{
									fReturn = DeleteFile(szBuf);
								}
							}
						}
					} while (/*fReturn && */FindNextFile(hFile, &findData));

					FindClose(hFile);
				}
			}

			return (fReturn == TRUE) ? true : false;
		}

		bool Directory::IsEmpty(const std::wstring& path)
		{
			return PathIsDirectoryEmpty(path.c_str()) == TRUE ? true : false;
		}
	}
}
