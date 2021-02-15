#pragma once

#include <System/Net/Http/HttpContent.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class StringContent
				: public HttpContent
			{
			public:
				StringContent(const std::string& content)
				{

				}

				StringContent(const std::string& content, const std::string& mediaType)
					: m_content(content)
					, m_mediaType(mediaType)
				{

				}

			protected:
				std::string m_content;
				std::string m_mediaType;
			};
		}
	}
}