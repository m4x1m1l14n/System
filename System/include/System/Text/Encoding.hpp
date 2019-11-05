#pragma once

#ifdef _WIN32

#include <string>

namespace System
{
	namespace Text
	{
		class Encoding
		{
		public:
			static std::wstring ToUTF16(const char * szText, size_t len);
			static std::wstring ToUTF16(const std::string& s);
			static std::string ToUTF8(const wchar_t* wszText, size_t len);
			static std::string ToUTF8(const std::wstring& s);
		};
	}
}

#endif // !_WIN32
