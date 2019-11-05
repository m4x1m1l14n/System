#include <System/Text/Encoding.hpp>

#ifdef _WIN32

#include <Windows.h>

namespace System
{
	namespace Text
	{
		std::wstring Encoding::ToUTF16(const char * szText, size_t len)
		{
			std::wstring outStr;

			int dstLen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, szText, static_cast<int>(len), nullptr, 0);
			if (dstLen > 0)
			{
				auto utf16Buff = reinterpret_cast<wchar_t*>(::LocalAlloc(0, dstLen * sizeof(wchar_t)));
				if (utf16Buff)
				{
					dstLen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, szText, static_cast<int>(len), utf16Buff, dstLen);
					if (dstLen > 0)
					{
						outStr = std::wstring(utf16Buff, dstLen);
					}

					::LocalFree(utf16Buff);
				}
			}

			return outStr;
		}

		std::wstring Encoding::ToUTF16(const std::string& s)
		{
			return ToUTF16(s.c_str(), s.length());
		}

		std::string Encoding::ToUTF8(const wchar_t* wszText, size_t len)
		{
			std::string outStr;

#if (WINVER >= 0x0600)
			DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#else
			DWORD dwConversionFlags = 0;
#endif

			int dstLen = ::WideCharToMultiByte(CP_UTF8, dwConversionFlags, wszText, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
			if (dstLen > 0)
			{
				auto utf8Buff = reinterpret_cast<char*>(::LocalAlloc(0, dstLen * sizeof(char)));
				if (utf8Buff)
				{
					dstLen = ::WideCharToMultiByte(CP_UTF8, dwConversionFlags, wszText, static_cast<int>(len), utf8Buff, dstLen, nullptr, nullptr);
					if (dstLen > 0)
					{
						outStr = std::string(utf8Buff, dstLen);
					}

					::LocalFree(utf8Buff);
				}
			}

			return outStr;
		}

		std::string Encoding::ToUTF8(const std::wstring& s)
		{
			return ToUTF8(s.c_str(), s.length());
		}
	}
}

#endif // !_WIN32
