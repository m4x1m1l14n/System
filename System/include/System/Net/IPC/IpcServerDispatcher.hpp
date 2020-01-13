#ifndef __IPC_SERVER_DISPATCHER_HPP__
#define __IPC_SERVER_DISPATCHER_HPP__

#include <System/Net/IPC/IpcDefines.hpp>

#include <System/Net/Sockets/Socket.hpp>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcServerDispatcher
			{
			public:
				virtual void IpcServer_Opened() { }
				virtual void IpcServer_Closed() { }

				virtual void IpcServer_ClientConnected(const IpcClientId clientId) { }
				virtual void IpcServer_OnMessage(const IpcClientId/* clientId*/, const IpcMessageId/* messageId*/, const std::string&/* message*/) { }
				virtual void IpcServer_ClientDisconnected(const IpcClientId clientId) { }

				virtual void IpcServer_EncryptPayload(std::string&/* payload*/) { }
				virtual void IpcServer_DecryptPayload(std::string&/* payload*/) { }
				virtual void IpcServer_OnError(const std::exception&/* ex*/) { }
			};
		}
	}
}

#endif // __IPC_SERVER_DISPATCHER_HPP__
