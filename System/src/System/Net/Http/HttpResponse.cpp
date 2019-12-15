#include <System/Net/Http/HttpResponse.hpp>

#include <sstream>
#include <regex>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			HttpResponse::HttpResponse(const std::string & data, const System::Net::Sockets::Socket_ptr socket)
			{
				size_t tail = data.find(HttpHeaders::HeadersTerminator);
				if (tail == std::string::npos)
				{
					throw std::invalid_argument("Header data is not complete");
				}

				size_t pos = data.find(HttpHeaders::HeadersSeparator);
				if (pos == std::string::npos)
				{
					throw std::invalid_argument("No header separator found!");
				}

				m_statusLine = HttpStatusLine(data.substr(0, pos));
				m_headers = HttpHeaders(data.substr(pos + HttpHeaders::HeadersSeparator.length(), tail));
				m_content = HttpContent(data.substr(tail + HttpHeaders::HeadersTerminator.length()), socket);
			}

			HttpResponse::HttpResponse(const std::string& statusLineData, const std::string& headersData)
				: HttpResponse(statusLineData, headersData, HttpContent())
			{

			}

			HttpResponse::HttpResponse(const std::string& statusLineData, const std::string& headersData, const HttpContent& content)
				: m_statusLine(HttpStatusLine(statusLineData))
				, m_headers(HttpHeaders(headersData))
				, m_content(content)
			{
				
			}

			HttpResponse::HttpResponse()
			{

			}

			HttpResponse::HttpResponse(const std::string & data)
			{
				size_t tail = data.find(HttpHeaders::HeadersTerminator);
				if (tail == std::string::npos)
				{
					throw std::invalid_argument("Header data is not complete");
				}

				size_t pos = data.find(HttpHeaders::HeadersSeparator);
				if (pos == std::string::npos)
				{
					throw std::invalid_argument("No header separator found!");
				}

				m_statusLine = HttpStatusLine(data.substr(0, pos));
				m_headers = HttpHeaders(data.substr(pos + HttpHeaders::HeadersSeparator.length(), tail));
				m_content = HttpContent(data.substr(tail));
			}

			HttpResponse HttpResponse::Parse(const std::string & data)
			{
				size_t pos = data.find(HttpHeaders::HeadersSeparator);
				if (pos == std::string::npos)
				{
					throw std::invalid_argument("No header separator found!");
				}

				return HttpResponse(
					data.substr(0, pos),
					data.substr(pos + HttpHeaders::HeadersSeparator.length())
				);
			}

			bool HttpResponse::TryParse(const std::string & data, HttpResponse& response)
			{
				auto success = false;

				try
				{
					response = HttpResponse::Parse(data);
				}
				catch (const std::exception&)
				{

				}

				return success;
			}

			const HttpStatusLine & HttpResponse::StatusLine() const
			{
				return m_statusLine;
			}

			HttpStatusLine HttpResponse::StatusLine()
			{
				return m_statusLine;
			}

			const HttpHeaders & HttpResponse::Headers() const
			{
				return m_headers;
			}

			HttpHeaders HttpResponse::Headers()
			{
				return m_headers;
			}

			HttpContent & HttpResponse::Content()
			{
				return m_content;
			}

			bool HttpResponse::IsSuccessStatusCode() const
			{
				return m_statusLine.StatusCode() == HttpStatusCode::Ok;
			}

			std::string HttpResponse::operator[](const std::string & header) const
			{
				return this->m_headers[header];
			}

			HttpStatusCode HttpResponse::StatusCode() const
			{
				return m_statusLine.StatusCode();
			}

			HttpStatusLine::HttpStatusLine()
			{

			}

			HttpStatusLine::HttpStatusLine(const std::string & data)
			{
				const std::string pattern = R"(^(\w+)\/([\w\.]+)\s(\d+)\s([\w\s]+)$)";

				std::regex re(pattern);
				std::smatch matches;

				if (std::regex_match(data, matches, re))
				{
					m_protocol = matches[1].str();
					m_version = System::Version(matches[2].str());
					m_statusCode = static_cast<HttpStatusCode>(std::atoi(matches[3].str().c_str()));
					m_statusText = matches[4].str();
				}
				else
				{
					throw std::invalid_argument("\"" + data + "\" is not a valid status line");
				}
			}

			const std::string & HttpStatusLine::Protocol() const
			{
				return m_protocol;
			}

			std::string HttpStatusLine::Protocol()
			{
				return m_protocol;
			}

			const System::Version & HttpStatusLine::Version() const
			{
				return m_version;
			}

			System::Version HttpStatusLine::Version()
			{
				return m_version;
			}

			HttpStatusCode HttpStatusLine::StatusCode() const
			{
				return m_statusCode;
			}

			const std::string & HttpStatusLine::StatusText() const
			{
				return m_statusText;
			}

			std::string HttpStatusLine::StatusText()
			{
				return m_statusText;
			}

			std::string HttpStatusLine::ToString() const
			{
				std::stringstream ss;

				// TODO Version.ToString(2) when std::string
				ss << m_protocol << "/" << m_version.getMajor() << "." << m_version.getMinor() << " " << static_cast<int>(m_statusCode) << " " << m_statusText;

				return ss.str();
			}
		}
	}
}
