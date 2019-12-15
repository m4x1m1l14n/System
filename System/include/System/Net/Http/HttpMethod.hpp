#pragma once

#include <string>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpMethod
			{
			public:
				HttpMethod(const std::string& method);
				
				// Predefined HTTP methods
				static const HttpMethod Get;
				static const HttpMethod Head;
				static const HttpMethod Post;
				static const HttpMethod Put;
				static const HttpMethod Delete;
				static const HttpMethod Connect;
				static const HttpMethod Options;
				static const HttpMethod Trace;
				static const HttpMethod Patch;

				std::string ToString() const;

			private:
				std::string m_method;
			};
		}
	}
}
