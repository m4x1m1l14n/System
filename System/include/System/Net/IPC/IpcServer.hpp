#ifndef __IPC_SERVER_HPP__
#define __IPC_SERVER_HPP__

#include <System/Net/Sockets/Socket.hpp>
#include <System/Net/IPC/IpcRequest.hpp>
#include <System/Net/IPC/IpcResponse.hpp>
#include <System/Net/IPC/IpcServerDispatcher.hpp>

#include <map>
#include <iostream>
#include <cassert>

// TODO using to source file!
using namespace System::Net::Sockets;

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			namespace details
			{
				struct Client
				{
					Socket_ptr socket;
					Timeout timeout;
					std::string rxBuffer;
					std::string txBuffer;
					std::deque<IpcMessage_ptr> txQueue;
					bool connected;
				};

				typedef std::shared_ptr<Client> Client_ptr;
			}

			class IpcServer
			{
			public:
				IpcServer(int port, IpcServerDispatcher *pDispatcher);

				/*
					virtual Dtor that stops IPC server if not stopped explicitly with call to Stop API
				*/
				virtual ~IpcServer();

				void Start();

				void Stop();

				IpcRequest_ptr CreateRequest(const IpcClientId clientId, const std::string& data);
				IpcRequest_ptr CreateRequest(const IpcClientId clientId, const std::string& data, const System::Timeout& timeout);

				IpcResponse_ptr CreateResponse(const IpcClientId clientId, const IpcRequest_ptr request, const std::string& data);
				IpcResponse_ptr CreateResponse(const IpcClientId clientId, const IpcRequest_ptr request, const std::string& data, const System::Timeout& timeout);

				std::string SendRequest(const IpcClientId clientId, const IpcRequest_ptr request);
				void SendResponse(const IpcClientId clientId, const IpcResponse_ptr response);

			protected:
				inline IpcMessageId GenerateRequestId(const IpcClientId clientId);

				void AcceptThread(int port);
				void WorkerThread();

			protected:
				int m_port;
				ManualResetEvent_ptr m_terminateEvent;
				IpcServerDispatcher *m_pDispatcher;

				std::map<const IpcClientId, details::Client_ptr> m_clients;
				std::mutex m_clientsLock;
				std::atomic<bool> m_clientsChanged;

				std::map<const IpcMessageId, std::shared_ptr<IpcRequest>> m_requests;
				std::mutex m_requestsLock;

				std::thread m_acceptThread;
				std::thread m_workerThread;

				// Statics
				static std::atomic<std::uint32_t> s_messageIdIterator;
			};

			typedef std::shared_ptr<IpcServer> IpcServer_ptr;
		}
	}
}

#endif // __IPC_SERVER_HPP__
