#include <System/Net/Sockets/SocketException.hpp>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			SocketException::SocketException(unsigned int errorCode)
				:SocketException(errorCode, std::string())
			{

			}

			SocketException::SocketException(unsigned int errorCode, const std::string& message)
				: m_errorCode(errorCode)
			{
				m_message = "Error code: " + std::to_string(errorCode);
				if (!message.empty())
				{
					m_message += ". " + message;
				}
			}

			SocketException::~SocketException() throw()
			{

			}

			const char * SocketException::what() const throw()
			{
				return m_message.c_str();
			}

			unsigned int SocketException::getErrorCode() const throw()
			{
				return m_errorCode;
			}
		}
	}
}
