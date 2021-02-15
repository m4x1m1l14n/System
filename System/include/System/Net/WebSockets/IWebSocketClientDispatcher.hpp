#pragma once

#include <string>

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			class IWebSocketClientDispatcher
			{
			public:
				virtual ~IWebSocketClientDispatcher() { }

				virtual void IWebSocketClient_OnConnected() { }
				virtual void IWebSocketClient_OnMessage(const std::string&/* message*/) { }
				virtual void IWebSocketClient_OnDisconnected() { }
			};
		}
	}
}
