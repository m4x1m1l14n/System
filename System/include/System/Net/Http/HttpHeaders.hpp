#pragma once

#include <string>
#include <unordered_map>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpHeaders
			{
			public:
				static const std::string HeadersSeparator;
				static const std::string HeadersTerminator;
				static const uint64_t MaxHeaderSize;

			public:
				HttpHeaders();
				HttpHeaders(const std::string& data);

				HttpHeaders(const HttpHeaders& rhs);
				HttpHeaders& operator=(const HttpHeaders& rhs);
				
				// NOTE
				//	Comment out destructor to enable default copy ctor & assignment operator
				// virtual ~HttpHeaders();

				HttpHeaders& Add(std::string key, std::string value);
				bool Contains(const std::string& key);

				std::string operator[](const std::string& key) const;

				std::string ToString() const;

			private:
				void Copy(const HttpHeaders& rhs);

			private:
				std::unordered_map<std::string, std::string> m_fields;
			};
		}
	}
}
