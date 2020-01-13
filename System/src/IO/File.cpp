#include <System/IO/File.hpp>

#include <System/IO/Path.hpp>
#include <System/Text/Encoding.hpp>

#include <Windows.h>
#include <assert.h>

#include <sys/stat.h>
#include <system_error>

using namespace System::Text;

namespace System
{
	namespace IO
	{
		void File::Rename(const std::wstring& filePath, const std::wstring& newFileName, bool overwrite/* = false*/)
		{
			const auto& directoryPath = Path::GetDirectoryName(filePath);
			const auto& newFilePath = Path::Combine(directoryPath, newFileName);

			if (File::Exists(newFilePath) && overwrite)
			{
				File::Delete(newFilePath);
			}

			BOOL fSuccess = ::MoveFile(filePath.c_str(), newFilePath.c_str());
			if (fSuccess == FALSE)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"MoveFile() failed"
				);
			}
		}

		void File::Delete(const std::wstring& filePath)
		{
			BOOL fSuccess = ::DeleteFile(filePath.c_str());
			if (fSuccess == FALSE)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"DeleteFile() failed on file \"" + Encoding::ToUTF8(filePath) + "\""
				);
			}
		}

		bool File::TryDelete(const std::wstring & filePath)
		{
			try
			{
				File::Delete(filePath);

				return true;
			}
			catch (...){ }

			return false;
		}

		void File::DeleteIfExists(const std::wstring & filePath)
		{
			if (File::Exists(filePath))
			{
				File::Delete(filePath);
			}
		}

		bool File::Copy(const std::wstring& sourceFileName, const std::wstring& destFileName, bool overwrite /*= false*/)
		{
			return (::CopyFile(
				sourceFileName.c_str(),
				destFileName.c_str(),
				overwrite ? FALSE : TRUE
			) == TRUE) ? true : false;
		}

		std::string File::ReadAllText(const std::wstring & fileName)
		{
			std::string fileContent;

			HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwFileSize = GetFileSize(hFile, NULL);
				if (dwFileSize != INVALID_FILE_SIZE)
				{
					DWORD dwBytesRead = 0;

					auto pBuffer = new char[dwFileSize];
					if (pBuffer) {
						if (::ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL)) {
							if (dwFileSize == dwBytesRead) {
								fileContent = std::string(pBuffer, dwFileSize);
							}
						}

						delete[] pBuffer;
					}
				}

				CloseHandle(hFile);
			}

			return fileContent;
		}

		bool File::WriteAllText(const std::wstring & fileName, const std::string & text)
		{
			bool success = false;
			HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwNumberOfBytesWritten = 0;

				success = WriteFile(hFile, text.c_str(), (DWORD)(text.length() /** sizeof(WCHAR)*/), &dwNumberOfBytesWritten, nullptr) ? true : false;

				CloseHandle(hFile);
			}

			return success;
		}

		__int64 File::GetSize(const std::wstring & fileName)
		{
			LPSECURITY_ATTRIBUTES pSecurityAttributes = nullptr;
			HANDLE hTemplateFile = nullptr;

			HANDLE hFile = ::CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, pSecurityAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, hTemplateFile);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"CreateFile() returned INVALID_HANDLE_VALUE on file \"" + Encoding::ToUTF8(fileName) + "\"."
				);
			}

			LARGE_INTEGER liFileSize;

			BOOL fReturn = GetFileSizeEx(hFile, &liFileSize);
			if (!fReturn)
			{
				::CloseHandle(hFile);

				throw std::system_error(
					std::error_code(::GetLastError(), std::system_category()),
					"GetFileSizeEx() returned FALSE."
				);
			}

			::CloseHandle(hFile);

			return liFileSize.QuadPart;
		}

		bool File::Move(const std::wstring & srcFileName, const std::wstring & dstFileName)
		{
			DWORD dwFlags = MOVEFILE_REPLACE_EXISTING;

			return ::MoveFileEx(
				srcFileName.c_str(),
				dstFileName.c_str(),
				dwFlags
			) == TRUE ? true : false;
		}

		std::string File::Checksum(const std::wstring & file)
		{
			std::string checksum;

			HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				BCRYPT_ALG_HANDLE hAlgorithm = nullptr;

				NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
				if (status == ERROR_SUCCESS)
				{
					DWORD dwBytesDone = 0;
					DWORD dwObjectLength = 0;

					status = BCryptGetProperty(hAlgorithm, BCRYPT_OBJECT_LENGTH, (BYTE*)&dwObjectLength, sizeof(dwObjectLength), &dwBytesDone, 0);
					if (status == ERROR_SUCCESS)
					{
						DWORD dwHashLength = 0;
						status = BCryptGetProperty(hAlgorithm, BCRYPT_HASH_LENGTH, (BYTE*)&dwHashLength, sizeof(dwHashLength), &dwBytesDone, 0);
						if (status == ERROR_SUCCESS)
						{
							PBYTE pbHashObject = nullptr;
							PBYTE pbHash = nullptr;

							pbHashObject = (PBYTE)LocalAlloc(LMEM_FIXED, dwObjectLength);
							if (pbHashObject)
							{
								pbHash = (PBYTE)LocalAlloc(LMEM_FIXED, dwHashLength);
								if (pbHash)
								{
									BCRYPT_HASH_HANDLE hHash = nullptr;

									status = BCryptCreateHash(hAlgorithm, &hHash, pbHashObject, dwObjectLength, nullptr, 0, 0);
									if (status == ERROR_SUCCESS)
									{
										BOOL fSuccess = FALSE;

										BYTE pBuffer[4096];
										DWORD dwBytesRead = 0;

										while ((fSuccess = ReadFile(hFile, pBuffer, ARRAYSIZE(pBuffer), &dwBytesRead, nullptr)) == TRUE && dwBytesRead > 0)
										{
											status = BCryptHashData(hHash, pBuffer, dwBytesRead, 0);
											if (status != ERROR_SUCCESS)
											{
												fSuccess = FALSE;

												break;
											}
										}

										if (fSuccess)
										{
											status = BCryptFinishHash(hHash, pbHash, dwHashLength, 0);
											if (status == ERROR_SUCCESS)
											{
												checksum = std::string(reinterpret_cast<char*>(pbHash), static_cast<size_t>(dwHashLength));
											}
										}

										BCryptDestroyHash(hHash);
									}

									LocalFree(pbHash);
								}

								LocalFree(pbHashObject);
							}
						}
					}

					BCryptCloseAlgorithmProvider(hAlgorithm, 0);
				}

				CloseHandle(hFile);
			}

			return checksum;
		}

		void File::Truncate(const std::wstring & file, const __int64 newSize)
		{
			std::exception_ptr pex;

			DWORD dwShareMode = 0; // No sharing
			LPSECURITY_ATTRIBUTES pSecurityAttributes = nullptr; // Default security attributes

			HANDLE hFile = ::CreateFile(file.c_str(), GENERIC_READ, dwShareMode, pSecurityAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				LARGE_INTEGER liFileSize;

				BOOL fReturn = ::GetFileSizeEx(hFile, &liFileSize);
				if (fReturn)
				{
					auto fileSize = liFileSize.QuadPart;
					if (fileSize > newSize)
					{
						LARGE_INTEGER li;

						li.QuadPart = fileSize - newSize;

						DWORD dwFilePointer = SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
						if (dwFilePointer != INVALID_SET_FILE_POINTER || ::GetLastError() == NO_ERROR)
						{
							auto directoryName = Path::GetDirectoryName(file);
						
							wchar_t wszTempFileName[MAX_PATH];

							UINT nReturn = GetTempFileName(directoryName.c_str(), L"TMP", 0, wszTempFileName);
							if (nReturn)
							{
								auto destinationFileName = Path::Combine(directoryName, wszTempFileName);

								HANDLE hDestination = CreateFile(destinationFileName.c_str(), GENERIC_WRITE, dwShareMode, pSecurityAttributes, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
								if (hDestination != INVALID_HANDLE_VALUE)
								{
									BYTE pBuffer[16 * 1024];
									DWORD dwBytesRead = 0;
									DWORD dwBytesWritten = 0;

									do
									{
										fReturn = ReadFile(hFile, pBuffer, ARRAYSIZE(pBuffer), &dwBytesRead, nullptr);
										if (fReturn)
										{
											if (dwBytesRead > 0)
											{
												fReturn = WriteFile(hDestination, pBuffer, dwBytesRead, &dwBytesWritten, nullptr);
												assert(dwBytesRead == dwBytesWritten);
												if (!fReturn)
												{
													pex = std::make_exception_ptr(
														std::system_error(
															std::error_code(::GetLastError(), std::system_category()),
															"WriteFile() returned FALSE on file \"" + Encoding::ToUTF8(destinationFileName) + "\"."
														)
													);
												}
											}
										}
										else
										{
											pex = std::make_exception_ptr(
												std::system_error(
													std::error_code(::GetLastError(), std::system_category()),
													"ReadFile() returned FALSE on file \"" + Encoding::ToUTF8(file) + "\"."
												)
											);

											break;
										}

									} while(fReturn && dwBytesRead > 0);

									::CloseHandle(hFile);
									hFile = INVALID_HANDLE_VALUE;

									::CloseHandle(hDestination);
									hDestination = INVALID_HANDLE_VALUE;

									if (fReturn)
									{
										fReturn = ::DeleteFile(file.c_str());
										if (fReturn)
										{
											fReturn = ::MoveFileEx(destinationFileName.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING);
											if (!fReturn && ::GetLastError() != ERROR_ALREADY_EXISTS)
											{
												pex = std::make_exception_ptr(
													std::system_error(
														std::error_code(::GetLastError(), std::system_category()),
														"MoveFile() returned FALSE on file \"" + Encoding::ToUTF8(destinationFileName) + "\"."
													)
												);
											}
										}
										else
										{
											pex = std::make_exception_ptr(
												std::system_error(
													std::error_code(::GetLastError(), std::system_category()),
													"DeleteFile() returned FALSE on file \"" + Encoding::ToUTF8(file) + "\"."
												)
											);
										}
									}
								}
								else
								{
									pex = std::make_exception_ptr(
										std::system_error(
											std::error_code(::GetLastError(), std::system_category()),
											"CreateFile() returned INVALID_HANDLE_VALUE on file \"" + Encoding::ToUTF8(file) + "\"."
										)
									);
								}
							}
							else
							{
								pex = std::make_exception_ptr(
									std::system_error(
										std::error_code(::GetLastError(), std::system_category()),
										"GetTempFileName() returned FALSE on file \"" + Encoding::ToUTF8(file) + "\"."
									)
								);
							}
						}
					}
				}
				else
				{
					pex = std::make_exception_ptr(
						std::system_error(
							std::error_code(::GetLastError(), std::system_category()),
							"GetFileSizeEx() returned FALSE on file \"" + Encoding::ToUTF8(file) + "\"."
						)
					);
				}

				if (hFile != INVALID_HANDLE_VALUE)
				{
					::CloseHandle(hFile);
				}
			}
			else
			{
				pex = std::make_exception_ptr(
					std::system_error(
						std::error_code(::GetLastError(), std::system_category()),
						"CreateFile() returned INVALID_HANDLE_VALUE on file \"" + Encoding::ToUTF8(file) + "\"."
					)
				);
			}

			if (pex)
			{
				std::rethrow_exception(pex);
			}
		}

		bool File::Exists(const std::wstring& file)
		{
			DWORD dwAttrib = GetFileAttributes(file.c_str());
			if (dwAttrib != INVALID_FILE_ATTRIBUTES)
			{
				return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ? false : true;
			}

			return false;
		}
	}
}
