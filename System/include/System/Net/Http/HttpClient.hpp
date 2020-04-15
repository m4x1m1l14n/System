#pragma once

#include <System/Net/Url.hpp>
#include <System/Net/Http/HttpResponse.hpp>
#include <System/Net/Http/HttpRequest.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpClient
			{
			public:
				HttpClient();

				std::string GetString(const System::Net::Url& url);
				std::string PostString(const System::Net::Url& url);

				HttpResponse Get(const System::Net::Url& url);
				HttpResponse Post(const System::Net::Url& url, const HttpContent& data) const;
			};
		}
	}
}
