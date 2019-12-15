#pragma once

#include <System/Version.hpp>
#include <System/Net/Sockets/Socket.hpp>
#include <System/Net/Url.hpp>
#include <System/Net/HttpStatusCode.hpp>
#include <System/Net/Http/HttpHeaders.hpp>
#include <System/Net/Http/HttpContent.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpStatusLine
			{
			public:
				HttpStatusLine();
				HttpStatusLine(const std::string& data);

				const std::string& Protocol() const;
				std::string Protocol();

				const System::Version& Version() const;
				System::Version Version();

				HttpStatusCode StatusCode() const;

				const std::string& StatusText() const;
				std::string StatusText();

				std::string ToString() const;

			private:
				std::string m_protocol;
				System::Version m_version;
				HttpStatusCode m_statusCode;
				std::string m_statusText;
			};

			class HttpResponse
			{
			public:
				friend class HttpClient;

				HttpResponse();
				HttpResponse(const std::string& data);

				static HttpResponse Parse(const std::string& data);
				static bool TryParse(const std::string& data, HttpResponse& response);

				const HttpStatusLine& StatusLine() const;
				HttpStatusLine StatusLine();

				const HttpHeaders& Headers() const;
				HttpHeaders Headers();
				
				HttpContent& Content();

				bool IsSuccessStatusCode() const;

				std::string operator[](const std::string& header) const;
				HttpStatusCode StatusCode() const;

			private:
				HttpResponse(const std::string& data, const System::Net::Sockets::Socket_ptr socket);
				HttpResponse(const std::string& statusLineData, const std::string& headersData);
				HttpResponse(const std::string& statusLineData, const std::string& headersData, const HttpContent& content);

			private:
				HttpStatusLine m_statusLine;
				HttpHeaders m_headers;
				HttpContent m_content;
			};
		}
	}
}
