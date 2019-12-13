#include <System/String.hpp>

namespace System
{
	namespace String
	{
		std::vector<std::string> Split(const std::string & str, const char c)
		{
			return Split<char>(str, c);
		}

		std::vector<std::wstring> Split(const std::wstring & str, const wchar_t c)
		{
			return Split<wchar_t>(str, c);
		}

		std::string Remove(const std::string & str, const std::string & needle)
		{
			return Remove<char>(str, needle);
		}

		std::string RemoveAll(const std::string & str, const std::string & needle)
		{
			return RemoveAll<char>(str, needle);
		}

		std::wstring RemoveAll(const std::wstring & str, const std::wstring & needle)
		{
			return RemoveAll<wchar_t>(str, needle);
		}

		/*
		std::string RemoveAll(std::string str, const std::vector<std::string>& needles)
		{
			return RemoveAll<char>(str, needles);
		}

		std::wstring RemoveAll(std::wstring str, const std::vector<std::wstring>& needles)
		{
			return RemoveAll<wchar_t>(str, needles);
		}
		*/

		std::string Remove(const std::string & str, const std::vector<char>& chars)
		{
			return Remove<char>(str, chars);
		}

		std::wstring Remove(const std::wstring & str, const std::wstring & needle)
		{
			return Remove<wchar_t>(str, needle);
		}

		std::wstring Remove(const std::wstring & str, const std::vector<wchar_t>& chars)
		{
			return Remove<wchar_t>(str, chars);
		}

		std::vector<std::string> Split(const std::string & str, const std::string & delim)
		{
			return Split<char>(str, delim);
		}

		std::vector<std::wstring> Split(const std::wstring & str, const std::wstring & delim)
		{
			return Split<wchar_t>(str, delim);
		}
	}
}
