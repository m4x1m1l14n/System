#include <System\Diagnostics\FileVersionInfo.hpp>

#include <Windows.h>

unsigned char* _GetFileVersionInfo(const std::wstring& fileName)
{
	DWORD dwHandle = 0;
	DWORD dwVersionInfo = ::GetFileVersionInfoSize(fileName.c_str(), &dwHandle);
	if (dwVersionInfo > 0)
	{
		auto hHeap = ::GetProcessHeap();

		auto pFileInfo = reinterpret_cast<unsigned char*>(::HeapAlloc(hHeap, 0, dwVersionInfo));
		if (!pFileInfo)
		{
			throw std::bad_alloc();
		}

		auto fSuccess = ::GetFileVersionInfo(fileName.c_str(), dwHandle, dwVersionInfo, pFileInfo);
		if (!fSuccess)
		{
			auto dwLastError = ::GetLastError();

			::HeapFree(hHeap, 0, pFileInfo);

			auto ec = std::error_code(dwLastError, std::system_category());

			throw std::system_error(ec, "GetFileVersionInfo() failed");
		}

		return pFileInfo;
	}
	else
	{
		auto ec = std::error_code(::GetLastError(), std::system_category());

		throw std::system_error(ec, "GetFileVersionInfoSize() failed");
	}
}

namespace m4x1m1l14n
{
	namespace System
	{
		namespace Diagnostics
		{
			FileVersionInfo::FileVersionInfo(unsigned char* pFileVersionInfo)
				: m_pFileVersionInfo(pFileVersionInfo)
			{
			}

			FileVersionInfo::~FileVersionInfo()
			{
				if (m_pFileVersionInfo)
				{
					::HeapFree(::GetProcessHeap(), 0, m_pFileVersionInfo);
				}
			}

			::System::Version FileVersionInfo::GetFileVersion() const
			{
				LPBYTE* lpBuffer = nullptr;
				UINT uLen = 0;

				if (VerQueryValue(m_pFileVersionInfo, L"\\", reinterpret_cast<LPVOID*>(&lpBuffer), &uLen))
				{
					if (uLen)
					{
						auto verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuffer);
						if (verInfo->dwSignature == 0xfeef04bd)
						{
							return ::System::Version
							(
								(verInfo->dwFileVersionMS >> 16) & 0xffff,
								(verInfo->dwFileVersionMS >> 0) & 0xffff,
								(verInfo->dwFileVersionLS >> 16) & 0xffff,
								(verInfo->dwFileVersionLS >> 0) & 0xffff
							);
						}
					}
				}
				else
				{
					throw std::runtime_error("VerQueryValue() failed");
				}

				return ::System::Version();
			}

			FileVersionInfo FileVersionInfo::GetVersionInfo(const std::wstring & fileName)
			{
				auto pFileVersionInfo = _GetFileVersionInfo(fileName);

				return FileVersionInfo(pFileVersionInfo);
			}
		}
	}
}
