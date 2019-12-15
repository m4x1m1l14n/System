#pragma once

#include <System/Version.hpp>
#include <System/Net/Http/HttpMethod.hpp>
#include <System/Net/Url.hpp>
#include <System/Net/Http/HttpHeaders.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpRequest
			{
			public:
				HttpRequest(const HttpMethod& method, const std::string& url, const System::Version& version = System::Version(1, 1));

				HttpMethod Method();
				std::string RequestUrl();
				HttpHeaders& Headers();

				std::string ToString() const;

			private:
				HttpMethod m_method;
				std::string m_requestUrl;
				HttpHeaders m_headers;
				// HTTP version
				System::Version m_version;
			};
		}
	}
}
