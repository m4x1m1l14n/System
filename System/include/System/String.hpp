#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace m4x1m1l14n {
	namespace System {
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
}
