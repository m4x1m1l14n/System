#include <System/Net/Http/HttpHeaders.hpp>

#include <System/String.hpp>

#include <sstream>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			const std::string HttpHeaders::HeadersSeparator = "\r\n";
			const std::string HttpHeaders::HeadersTerminator = "\r\n\r\n";
			const uint64_t HttpHeaders::MaxHeaderSize = 16 * 1024; // 16 kb

			HttpHeaders::HttpHeaders()
			{

			}

			HttpHeaders::HttpHeaders(const std::string & data)
			{
				size_t pos = 0, prev = 0;

				while ((pos = data.find(HeadersSeparator, prev)) != std::string::npos)
				{
					if (pos == prev)
					{
						break;
					}

					auto i = data.find(':', prev);

					const auto& key = data.substr(prev, i - prev);
					const auto& value = data.substr(i + 1, pos - i - 1);

					this->Add(key, value);

					pos += HeadersSeparator.length();
					prev = pos;
				}
			}

			HttpHeaders::HttpHeaders(const HttpHeaders & rhs)
			{
				this->Copy(rhs);
			}

			HttpHeaders & HttpHeaders::operator=(const HttpHeaders & rhs)
			{
				this->Copy(rhs);

				return *this;
			}

			HttpHeaders & HttpHeaders::Add(std::string key, std::string value)
			{
				this->m_fields[String::Trim(key)] = String::Trim(value);

				// Enable chaining
				return *this;
			}

			bool HttpHeaders::Contains(const std::string & key)
			{
				return (this->m_fields.find(key) != this->m_fields.end());
			}

			std::string HttpHeaders::operator[](const std::string & key) const
			{
				const auto iter = this->m_fields.find(key);
				if (iter == this->m_fields.end())
				{
					throw std::invalid_argument("Key \"" + key + "\" not found!");
				}

				return iter->second;
			}

			std::string HttpHeaders::ToString() const
			{
				std::stringstream ss;

				for (const auto& field : m_fields)
				{
					ss << field.first << ": " << field.second << "\r\n";
				}

				return ss.str();
			}

			void HttpHeaders::Copy(const HttpHeaders & rhs)
			{
				this->m_fields = rhs.m_fields;
			}
		}
	}
}
