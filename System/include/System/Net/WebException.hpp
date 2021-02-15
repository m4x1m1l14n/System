#pragma once

#include <string>

#include <System/Net/HttpStatusCode.hpp>

namespace System
{
	namespace Net
	{
		class WebException
			: public std::exception
		{
		public:
			explicit WebException(const HttpStatusCode statusCode)
				: m_statusCode(statusCode)
			{
				m_message = std::to_string(static_cast<int>(m_statusCode));
			}

			virtual ~WebException() throw()
			{

			}

			virtual const char* what() const throw()
			{
				return m_message.c_str();
			}

			HttpStatusCode getStatusCode() const throw() { return m_statusCode; }

		protected:
			HttpStatusCode m_statusCode;
			std::string m_message;
		};
	}
}
