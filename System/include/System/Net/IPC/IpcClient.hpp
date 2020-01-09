#ifndef __IPC_CLIENT_HPP__
#define __IPC_CLIENT_HPP__

#include <System/Net/IPC/IpcCommon.hpp>
#include <System/Net/IPC/IpcClientDispatcher.hpp>

#include <System/Net/IPC/IpcRequest.hpp>
#include <System/Net/IPC/IpcResponse.hpp>

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
				IpcClient(int port, IpcClientDispatcher *pDispatcher);
				IpcClient(const std::string& host, int port, IpcClientDispatcher *pDispatcher);

				virtual ~IpcClient();

				void Start();
				void Stop();

				// Getters
				IpcClientId ClientId() const;

				std::string SendRequest(const std::string& data);
				std::string SendRequest(const std::string& data, const TimeSpan& timeout);

				void SendResponse(const IpcMessageId messageId, const std::string& data);
				void SendResponse(const IpcMessageId messageId, const std::string& data, const TimeSpan& timeout);

			private:
				inline IpcMessageId GenerateRequestId(const IpcClientId clientId);

				void RxThread(std::string host, int port);
				void TxThread();

				void Register(System::Net::Sockets::Socket_ptr socket);

				void WriteMessage(System::Net::Sockets::Socket_ptr socket, const std::string& message);
				std::string ReadMessage();

				void DispatchExpiredMessages();
				void DispatchExpiredRequests();
				void CancelPendingMessages();

				std::shared_ptr<IpcMessage> PopQueueMessage(size_t& prevQueueSize);

				System::Net::Sockets::Socket_ptr GetSocket();
				void SetSocket(System::Net::Sockets::Socket_ptr socket);

			private:
				IpcClientId m_clientId;

				// Replace with Url class
				std::string m_host;
				int m_port;

				IpcClientDispatcher* m_pDispatcher;

				ManualResetEvent_ptr m_terminateEvent;

				System::Net::Sockets::Socket_ptr m_socket;
				std::mutex m_socketLock;

				std::deque<std::shared_ptr<IpcMessage>> m_queue;
				std::mutex m_queueLock;
				ManualResetEvent_ptr m_queueChangedEvent;

				std::map<const IpcMessageId, std::shared_ptr<IpcRequest>> m_requestQueue; // TODO Rename to m_requests
				std::mutex m_requestQueueLock; // TODO Rename m_requestsLock

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
