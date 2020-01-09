#include <System/Net/Http/HttpMethod.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			HttpMethod::HttpMethod(const std::string & method)
				: m_method(method)
			{

			}

			std::string HttpMethod::ToString() const
			{
				return m_method;
			}

			const HttpMethod HttpMethod::Get = HttpMethod("GET");
			const HttpMethod HttpMethod::Head = HttpMethod("HEAD");
			const HttpMethod HttpMethod::Post = HttpMethod("POST");
			const HttpMethod HttpMethod::Put = HttpMethod("PUT");
			const HttpMethod HttpMethod::Delete = HttpMethod("DELETE");
			const HttpMethod HttpMethod::Connect = HttpMethod("CONNECT");
			const HttpMethod HttpMethod::Options = HttpMethod("OPTIONS");
			const HttpMethod HttpMethod::Trace = HttpMethod("TRACE");
			const HttpMethod HttpMethod::Patch = HttpMethod("PATCH");
		}
	}
}
