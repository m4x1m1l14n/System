#ifndef __IPC_CLIENT_DISPATCHER_HPP__
#define __IPC_CLIENT_DISPATCHER_HPP__

#include <System/Net/IPC/IpcMessage.hpp>

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
				virtual void IpcClient_OnMessage(const IpcMessage_ptr/* message*/) { }
				virtual void IpcClient_OnDisconnected() { }
			};
		}
	}
}

#endif // __IPC_CLIENT_DISPATCHER_HPP__
