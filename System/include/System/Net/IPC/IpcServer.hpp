#ifndef __IPC_SERVER_HPP__
#define __IPC_SERVER_HPP__


#include <System/Net/Sockets/Socket.hpp>
#include <System/Net/IPC/IpcServerDispatcher.hpp>
#include <System/Net/IPC/IpcQueueItem.hpp>

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
					IpcClientId clientId;
					Timeout timeout;
					std::string rxBuffer;
					std::string txBuffer;
					std::deque<IpcQueueItem_ptr> txQueue;
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

				int Port() const;

				IpcMessage_ptr CreateRequest(const std::string& data);
				IpcMessage_ptr CreateResponse(const IpcMessage_ptr request, const std::string& data);

				IpcMessage_ptr SendRequest(const IpcClientId clientId, const IpcMessage_ptr request);
				IpcMessage_ptr SendRequest(const IpcClientId clientId, const IpcMessage_ptr request, const System::Timeout& timeout);

				void SendResponse(const IpcClientId clientId, const IpcMessage_ptr response);
				void SendResponse(const IpcClientId clientId, const IpcMessage_ptr response, const System::Timeout& timeout);

			protected:
				inline IpcMessageId GenerateRequestId();

				void AcceptThread(int port);
				void WorkerThread();

				void ProcessResponse(const IpcMessage_ptr response);

				void Invoke_Opened();
				void Invoke_ClientConnected(const IpcClientId clientId);
				void Invoke_ClientDisconnected(const IpcClientId clientId);
				void Invoke_OnMessage(const IpcClientId clientId, const IpcMessage_ptr message);
				void Invoke_EncryptPayload(std::string& payload);
				void Invoke_DecryptPayload(std::string& payload);
				void Invoke_OnError(const std::exception& ex);

			protected:
				int m_port;
				ManualResetEvent_ptr m_terminateEvent;
				IpcServerDispatcher *m_pDispatcher;

				std::map<const IpcClientId, details::Client_ptr> m_clients;
				std::mutex m_clientsLock;

				std::map<const IpcMessageId, details::IpcQueueRequestItem_ptr> m_requests;
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
