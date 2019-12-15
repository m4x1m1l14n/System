#pragma once

#include <System/Net/Sockets/SocketException.hpp>

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			class WebSocketException
				: public System::Net::Sockets::SocketException
			{
			public:
				explicit WebSocketException(unsigned int errorCode);

				explicit WebSocketException(unsigned int errorCode, const std::string& message);
			};
		}
	}
}
