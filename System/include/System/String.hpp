#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace System
{
	namespace String
	{
		template <typename _C, typename _Function>
		inline std::string Implode(const std::vector<_C>& v, const std::string& separator, _Function&& func)
		{
			std::stringstream ss;

			auto first = v.cbegin(), last = v.cend();
			if (first != last)
			{
				while (true)
				{
					ss << func(*first);
					++first;

					if (first == last)
					{
						break;
					}

					ss << separator;
				}
			}

			return ss.str();
		}

		template <typename _T>
		inline std::basic_string<_T> Implode(const std::vector<std::basic_string<_T>>& v, const std::basic_string<_T>& separator)
		{
			std::basic_stringstream<_T> ss;

			auto first = v.cbegin(), last = v.cend();
			if (first != last)
			{
				while (true)
				{
					ss << *first;
					++first;

					if (first == last)
					{
						break;
					}

					ss << separator;
				}
			}

			return ss.str();
		}

		template <typename _T>
		inline std::basic_string<_T>& UpperCase(std::basic_string<_T>& s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });

			return s;
		}

		template <typename _T>
		inline std::basic_string<_T> UpperCaseCopy(const std::basic_string<_T>& s)
		{
			auto copy = s;

			String::UpperCase(copy);

			return copy;
		}

		template <typename _T>
		inline std::basic_string<_T>& LowerCase(std::basic_string<_T>& s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

			return s;
		}

		template <typename _T>
		inline std::basic_string<_T> LowerCaseCopy(const std::basic_string<_T>& s)
		{
			auto copy = s;

			String::LowerCase(copy);

			return copy;
		}

		inline std::string Implode(const std::vector<std::string>& v, const char* separator)
		{
			return String::Implode(v, std::string(separator));
		}

		inline std::wstring Implode(const std::vector<std::wstring>& v, const wchar_t* separator)
		{
			return String::Implode(v, std::wstring(separator));
		}
	}
}
