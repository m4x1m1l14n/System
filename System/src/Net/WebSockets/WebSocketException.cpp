#include <System/Net/WebSockets/WebSocketException.hpp>

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			WebSocketException::WebSocketException(unsigned int errorCode)
				: SocketException(errorCode)
			{
			}

			WebSocketException::WebSocketException(unsigned int errorCode, const std::string & message)
				: SocketException(errorCode, message)
			{

			}
		}
	}
}
