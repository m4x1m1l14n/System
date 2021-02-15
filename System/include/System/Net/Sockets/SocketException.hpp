#ifndef __SOCKET_EXCEPTION_HPP__
#define __SOCKET_EXCEPTION_HPP__

#include <string>
#include <exception>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			// TODO Inherit from system_error or format message based on socket error code
			class SocketException
				: public std::exception
			{
			public:
				explicit SocketException(unsigned int errorCode);

				explicit SocketException(unsigned int errorCode, const std::string& message);

				virtual ~SocketException() throw ();

				virtual const char* what() const throw ();

				virtual unsigned int getErrorCode() const throw();

				__declspec(property(get = getErrorCode)) unsigned int ErrorCode;

			protected:
				unsigned int m_errorCode;
				std::string m_message;
			};
		}
	}
}

#endif // __SOCKET_EXCEPTION_HPP__
