#pragma once

#ifdef _WIN32

#include <Windows.h>

#endif

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <type_traits>

namespace System
{
	namespace String
	{
		template <typename _T>
		std::vector<std::basic_string<_T>> Split(const std::basic_string<_T>& str, const _T c)
		{
			static_assert(std::is_same<_T, char>::value || std::is_same<_T, wchar_t>::value, "Only char or wchar_t template is accepted");

			std::vector<std::basic_string<_T>> splits;

			typename std::basic_string<_T>::size_type pos = str.find_first_of(c);
			decltype(pos) prevpos = 0;

			while (pos != std::basic_string<_T>::npos)
			{
				splits.push_back(str.substr(prevpos, pos - prevpos));
				prevpos = pos + 1;

				pos = str.find_first_of(c, prevpos);
			}

			splits.push_back(str.substr(prevpos));

			return splits;
		}

		std::vector<std::string> Split(const std::string& str, const char c);
		std::vector<std::wstring> Split(const std::wstring& str, const wchar_t c);

		template <typename _T>
		std::vector<std::basic_string<_T>> Split(const std::basic_string<_T>& str, const std::basic_string<_T>& delim)
		{
			static_assert(std::is_same<_T, char>::value || std::is_same<_T, wchar_t>::value, "Only char or wchar_t template is accepted");

			std::vector<std::basic_string<_T>> splits;

			typename std::basic_string<_T>::size_type pos = str.find(delim);
			decltype(pos) prevpos = 0;

			while (pos != std::basic_string<_T>::npos)
			{
				splits.push_back(str.substr(prevpos, pos - prevpos));
				prevpos = pos + delim.length();

				pos = str.find(delim, prevpos);
			}

			splits.push_back(str.substr(prevpos));

			return splits;
		}

		std::vector<std::string> Split(const std::string& str, const std::string& delim);
		std::vector<std::wstring> Split(const std::wstring& str, const std::wstring& delim);

		/*
			Removes first occurence of specified needle from input string
		*/
		template <typename _T>
		std::basic_string<_T> Remove(std::basic_string<_T> str, const std::basic_string<_T>& needle)
		{
			typename std::basic_string<_T>::size_type pos = str.find(needle);
 
			if (pos != std::string::npos)
			{
				str.erase(pos, needle.length());
			}

			return str;
		}

		std::string Remove(const std::string& str, const std::string& needle);
		std::wstring Remove(const std::wstring& str, const std::wstring& needle);

		/*
			Removes all characters from input string
		*/
		template <typename _T>
		std::basic_string<_T> Remove(std::basic_string<_T> str, const std::vector<_T>& chars)
		{
			for (const auto& c : chars)
			{
				str.erase(std::remove(str.begin(), str.end(), c), str.end());
			}

			return str;
		}

		std::string Remove(const std::string& str, const std::vector<char>& chars);
		std::wstring Remove(const std::wstring& str, const std::vector<wchar_t>& chars);

		/*
			Removes all occurences of specified needle from input string
		*/
		template <typename _T>
		std::basic_string<_T> RemoveAll(std::basic_string<_T> str, const std::basic_string<_T>& needle)
		{
			typename std::basic_string<_T>::size_type pos = std::string::npos;
 
			while ((pos = str.find(needle)) != std::string::npos)
			{
				str.erase(pos, needle.length());
			}

			return str;
		}

		std::string RemoveAll(const std::string& str, const std::string& needle);
		std::wstring RemoveAll(const std::wstring& str, const std::wstring& needle);

		/*
			Removes all occurences of all needles from input string
		*/
		/*
		template <typename _T>
		std::basic_string<_T> RemoveAll(std::basic_string<_T> str, const std::vector<std::basic_string<_T>>& needles)
		{
			for (const auto& needle : needles)
			{
				str = RemoveAll(str, needle);
			}
			
			return str;
		}

		std::string RemoveAll(std::string str, const std::vector<std::string>& needles);
		std::wstring RemoveAll(std::wstring str, const std::vector<std::wstring>& needles);
		*/

		// TODO Implement Replace
		// TODO Implement Join
		// TODO Implement Count

		inline std::wstring TrimStart(std::wstring s, const wchar_t* t = L" \t\n\r\f\v")
		{
			return s.erase(0, s.find_first_not_of(t));
		}

		inline std::wstring TrimEnd(std::wstring s, const wchar_t* t = L" \t\n\r\f\v")
		{
			return s.erase(s.find_last_not_of(t) + 1);
		}

		inline std::wstring Trim(std::wstring s, const wchar_t* t = L" \t\n\r\f\v")
		{
			return TrimStart(TrimEnd(s, t), t);
		}

		inline std::string TrimStart(std::string s, const char* t = " \t\n\r\f\v")
		{
			return s.erase(0, s.find_first_not_of(t));
		}

		inline std::string TrimEnd(std::string s, const char* t = " \t\n\r\f\v")
		{
			return s.erase(s.find_last_not_of(t) + 1);
		}

		inline std::string Trim(std::string s, const char* t = " \t\n\r\f\v")
		{
			return TrimStart(TrimEnd(s, t), t);
		}

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

		/*template <typename _T>
		inline std::basic_string<_T>& UpperCase(std::basic_string<_T>& s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](const _T c) { return static_cast<_T>(std::toupper(c)); });

			return s;
		}*/

		inline std::string& UpperCase(std::string& s)
		{
#ifdef _WIN32
			CharUpperBuffA(&s[0], static_cast<DWORD>(s.length()));
#else
			std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c)
			{
				return static_cast<char>(std::toupper(c));
			});
#endif
			
			return s;
		}

		inline std::wstring& UpperCase(std::wstring& s)
		{
#ifdef _WIN32
			CharUpperBuffW(&s[0], static_cast<DWORD>(s.length()));
#else
			std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c)
			{
				return std::towupper(c);
			});
#endif

			return s;
		}

		inline std::wstring UpperCaseCopy(const std::wstring& s)
		{
			auto copy = s;

			String::UpperCase(copy);

			return copy;
		}

		inline std::string UpperCaseCopy(const std::string& s)
		{
			auto copy = s;

			String::UpperCase(copy);

			return copy;
		}

		inline std::string& LowerCase(std::string& s)
		{
#ifdef _WIN32
			CharLowerBuffA(&s[0], static_cast<DWORD>(s.length()));
#else
			std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c)
			{
				return static_cast<char>(std::tolower(c)); 
			});
#endif

			return s;
		}

		inline std::wstring& LowerCase(std::wstring& s)
		{
#ifdef _WIN32
			CharLowerBuffW(&s[0], static_cast<DWORD>(s.length()));
#else
			std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c)
			{
				return std::towlower(c); 
			});
#endif

			return s;
		}

		inline std::string LowerCaseCopy(const std::string& s)
		{
			auto copy = s;

			String::LowerCase(copy);

			return copy;
		}

		inline std::wstring LowerCaseCopy(const std::wstring& s)
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

		template <typename _T>
		inline bool IsPalindrome(const std::basic_string<_T>& s)
		{
			return std::equal(s.begin(), s.begin() + s.length() / 2, s.rbegin());
		}

		template <typename _T>
		inline bool CompareCaseInsensitive(const std::basic_string<_T>& lhs, const std::basic_string<_T>& rhs)
		{
			// NOTE
			//	When ranges specified with first and last iterators for both input strings
			//	there is no need to check string sizes explicitly, because std::equal will do
			//	it for us.
			return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](const _T& a, const _T& b)
			{
				return std::tolower(a) == std::tolower(b);
			});
		}

		inline bool CompareCaseInsensitive(const std::wstring& lhs, const std::wstring& rhs)
		{
			return CompareCaseInsensitive<wchar_t>(lhs, rhs);
		}

		inline bool CompareCaseInsensitive(const std::string& lhs, const std::string& rhs)
		{
			return CompareCaseInsensitive<char>(lhs, rhs);
		}

#if defined(_WIN32) && 0
		inline bool CompareCaseInsensitive(const std::wstring& a, const std::wstring& b)
		{
			return (
				a.length() == b.length() &&
				_wcsnicmp(a.c_str(), b.c_str(), a.length()) == 0
				);
		}

		inline bool CompareCaseInsensitive(const std::string& a, const std::string& b)
		{
			return (
				a.length() == b.length() &&
				_strnicmp(a.c_str(), b.c_str(), a.length()) == 0
				);
		}
#endif // _WIN32
	}
}
