#ifndef __IPC_CLIENT_DISPATCHER_HPP__
#define __IPC_CLIENT_DISPATCHER_HPP__

#include <string>

#include <json11.hpp>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcClientDispatcher
			{
			public:
				// TODO Define as pure virtuals
				virtual void IpcClient_OnConnected() { }
				virtual void IpcClient_OnError(const std::string&/* error*/) { }
				virtual void IpcClient_OnMessage(const IpcMessageId/* id*/, const json11::Json&/* json*/) { }
				virtual void IpcClient_OnDisconnected() { }
			};
		}
	}
}

#endif // __IPC_CLIENT_DISPATCHER_HPP__
