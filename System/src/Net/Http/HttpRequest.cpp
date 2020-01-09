#include <System/Net/Http/HttpRequest.hpp>

#include <sstream>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			HttpRequest::HttpRequest(const HttpMethod& method, const std::string& requestUrl, const System::Version& version)
				: m_method(method)
				, m_requestUrl(requestUrl)
				, m_version(version)
			{
				
			}

			HttpMethod HttpRequest::Method()
			{
				return m_method;
			}

			std::string HttpRequest::RequestUrl()
			{
				return m_requestUrl;
			}

			HttpHeaders & HttpRequest::Headers()
			{
				return m_headers;
			}

			std::string HttpRequest::ToString() const
			{
				std::stringstream ss;

				ss
					<< m_method.ToString() << " " << ((m_requestUrl.empty() || m_requestUrl.front() != '/') ? ("/" + m_requestUrl) : m_requestUrl) << " HTTP/1.1\r\n"
					<< m_headers.ToString()
					<< "\r\n";

				return ss.str();
			}
		}
	}
}
