#ifndef __IPC_CLIENT_HPP__
#define __IPC_CLIENT_HPP__

#include <System/Net/IPC/IpcDefines.hpp>
#include <System/Net/IPC/IpcClientDispatcher.hpp>
#include <System/Net/IPC/IpcMessage.hpp>
#include <System/Net/IPC/IpcQueueItem.hpp>

#include <System/Net/Sockets/Socket.hpp>

#include <System/Threading/ManualResetEvent.hpp>
#include <System/Threading/AutoResetEvent.hpp>
#include <System/TimeSpan.hpp>

#include <atomic>
#include <thread>
#include <deque>
#include <map>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcClient
			{
			public:
				IpcClient(int port);
				IpcClient(int port, IpcClientDispatcher *pDispatcher);
				IpcClient(const std::string& host, int port, IpcClientDispatcher *pDispatcher);

				virtual ~IpcClient();

				void Start();
				void Stop();

				// Getters
				IpcClientId ClientId() const;

				IpcMessage_ptr CreateRequest(const std::string& data);
				IpcMessage_ptr CreateResponse(const IpcMessage_ptr request, const std::string& data);

				IpcMessage_ptr SendRequest(const IpcMessage_ptr request);
				IpcMessage_ptr SendRequest(const IpcMessage_ptr request, const Timeout& timeout);

				void SendResponse(const IpcMessage_ptr response);
				void SendResponse(const IpcMessage_ptr response, const Timeout& timeout);

			private:
				IpcClientId CreateClientId();
				inline IpcMessageId GenerateRequestId();

				void RxThread(std::string host, int port);
				void TxThread();

				void Register(System::Net::Sockets::Socket_ptr socket);

				void WriteMessage(System::Net::Sockets::Socket_ptr socket, const IpcMessage_ptr message, const System::Timeout& timeout);
				IpcMessage_ptr ReadMessage();

				void DispatchExpiredMessages();
				void DispatchExpiredRequests();
				void CancelPendingMessages();
				void ProcessResponse(const IpcMessage_ptr response);

				details::IpcQueueItem_ptr PopQueueItem(size_t& prevQueueSize);

				System::Net::Sockets::Socket_ptr GetSocket();
				void SetSocket(System::Net::Sockets::Socket_ptr socket);

				void Invoke_OnConnected();
				void Invoke_OnDisconnected();
				void Invoke_OnMessage(const IpcMessage_ptr message);
				void Invoke_OnError(const std::exception& ex);
				void Invoke_EncyptPayload(std::string& payload);
				void Invoke_DecryptPayload(std::string& payload);

			private:
				IpcClientId m_clientId;

				// Replace with Url class
				std::string m_host;
				int m_port;

				IpcClientDispatcher* m_pDispatcher;

				ManualResetEvent_ptr m_terminateEvent;

				System::Net::Sockets::Socket_ptr m_socket;
				std::mutex m_socketLock;

				std::deque<details::IpcQueueItem_ptr> m_queue;
				std::mutex m_queueLock;
				ManualResetEvent_ptr m_queueChangedEvent;

				std::map<const IpcMessageId, details::IpcQueueRequestItem_ptr> m_requests;
				std::mutex m_requestsLock;

				ManualResetEvent_ptr m_writeDoneEvent;
				ManualResetEvent_ptr m_connectedEvent;

				std::thread m_rxThread;
				std::thread m_txThread;

				// Buffer for incoming data from socket
				std::string m_buffer;

				static std::atomic<std::uint32_t> s_instanceCounter;
				static std::atomic<std::uint32_t> s_messageIdIterator;
			};

			typedef std::shared_ptr<IpcClient> IpcClient_ptr;
		}
	}
}

#endif // __IPC_CLIENT_HPP__
