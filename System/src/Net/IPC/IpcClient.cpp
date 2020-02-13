#include <System/Net/IPC/IpcClient.hpp>

#include "IpcCommon.hpp"

#include <iostream>


using namespace System::Threading;
using namespace System::Net::Sockets;


#ifdef _DEBUG

#include <sstream>

#define __DBG__(val)										\
do {														\
	std::stringstream __ss;									\
	__ss << val;											\
	__ss << " [" << __FILE__ << ": " << __LINE__ << "]";	\
	__ss << "\n";											\
	::OutputDebugStringA(__ss.str().c_str());				\
} while (0)

#else

#define __DBG__(val) ((void)0)

#endif


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcClientDefaultDispatcher
				: public IpcClientDispatcher
			{

			};

			
			// Statics
			std::atomic<std::uint32_t> IpcClient::s_instanceCounter = 1;
			std::atomic<std::uint32_t> IpcClient::s_messageIdIterator = 1;
			IpcClientDefaultDispatcher s_defaultDispatcher;


			IpcClient::IpcClient(int port)
				: IpcClient(std::string(), port, nullptr)
			{

			}

			IpcClient::IpcClient(int port, IpcClientDispatcher * pDispatcher)
				: IpcClient(std::string(), port, pDispatcher)
			{

			}

			IpcClient::IpcClient(const std::string & host, int port, IpcClientDispatcher * pDispatcher)
				: m_terminateEvent(std::make_shared<ManualResetEvent>())
				, m_queueChangedEvent(std::make_shared<ManualResetEvent>())
				, m_writeDoneEvent(std::make_shared<ManualResetEvent>(true))
				, m_connectedEvent(std::make_shared<ManualResetEvent>())
				, m_host(host.empty() ? "localhost" : host)
				, m_port(port)
			{
				m_pDispatcher = (pDispatcher != nullptr)
					? pDispatcher
					: &s_defaultDispatcher;

				if (m_port == 0)
				{
					throw std::invalid_argument("port number must be spcecified");
				}

				if (pDispatcher == nullptr)
				{
					throw std::invalid_argument("dispatcher cannot be null");
				}

				m_clientId = this->CreateClientId();
			}

			/**
				Class dtor

				Stops IPC client in case client is not stopped explicitly
				by calling Stop member
			*/
			IpcClient::~IpcClient()
			{
				this->Stop();
			}

			IpcClientId IpcClient::ClientId() const
			{
				return m_clientId;
			}

			IpcMessage_ptr IpcClient::CreateRequest(const std::string& data)
			{
				const auto id = this->GenerateRequestId();

				const auto& request = IpcMessage::CreateClientRequest(id, data);

				return request;
			}

			IpcMessage_ptr IpcClient::CreateResponse(const IpcMessage_ptr request, const std::string& data)
			{
				const auto id = request->Id();

				const auto& response = IpcMessage::CreateClientResponse(id, data);

				return response;
			}

			IpcMessage_ptr IpcClient::SendRequest(const IpcMessage_ptr request)
			{
				return this->SendRequest(request, Timeout::Infinite);
			}

			IpcMessage_ptr IpcClient::SendRequest(const IpcMessage_ptr request, const Timeout& timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				auto requestQueueItem = std::make_shared<details::IpcQueueRequestItem>(request, timeout);

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					m_queue.push_back(requestQueueItem);
				}

				m_queueChangedEvent->Set();

				return requestQueueItem->Wait();
			}

			void IpcClient::SendResponse(const IpcMessage_ptr response)
			{
				return this->SendResponse(response, Timeout::Infinite);
			}

			void IpcClient::SendResponse(const IpcMessage_ptr response, const Timeout & timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				const auto& responseQueueItem = std::make_shared<details::IpcQueueResponseItem>(response, timeout);

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					m_queue.push_back(responseQueueItem);
				}

				m_queueChangedEvent->Set();

				responseQueueItem->Wait();
			}

			/**
			 * Constructs IPC client ID
			 *
			 * Client ID is 64 bit unsigned integer which upper 32 bits contains process ID of process
			 * owning this instance and lower 32 bit part contains unique instance number of this IPC client.
			 *
			 *  _________________________________
			 * |    63 - 32    |     31 - 0      |
			 * |---------------------------------|
			 * |   Process ID  | Instance number |
			 * |---------------------------------|
			 *
			 * @return IPC client ID
			 */
			IpcClientId IpcClient::CreateClientId()
			{
				const auto processId = ::GetCurrentProcessId();
				const auto clientId = static_cast<IpcClientId>(processId) << 32 | s_instanceCounter++;

				return clientId;
			}

			/**
			 * Generates unique IPC client request ID
			 *
			 * IPC client request ID consists of IPC client request flag (MSB set to 0)
			 * and per instance unique message ID number
			 *
			 * @return IPC client request ID
			 */
			inline IpcMessageId IpcClient::GenerateRequestId()
			{
				return s_messageIdIterator++;
			}

			void IpcClient::RxThread(std::string host, int port)
			{
				int waitTime = 0;

				do
				{
					try
					{
						auto socket = std::make_shared<Socket>();

						socket->Connect(host, port);

						// Before enabling write & dispatching incoming messages first
						// register client to server
						this->Register(socket);

						// Attach connected socket to member variable
						this->SetSocket(socket);

						// Signal to transmitter thread that writes to socket can be done
						m_connectedEvent->Set();

						// Signal to dispatcher object, that client was successfully connected
						this->Invoke_OnConnected();

						// After successfull connect reset reconnect timeout to zero
						waitTime = 0;

						do
						{
							auto message = this->ReadMessage();

							if (message->IsResponse())
							{
								this->ProcessResponse(message);
							}
							else
							{
								this->Invoke_OnMessage(message);
							}

						} while (!m_terminateEvent->IsSet());
					}
					catch (const System::OperationCanceledException&)
					{
						// NOTE
						//	In case of operation cancelled, do not report error
					}
					catch (const std::exception& ex)
					{
						this->Invoke_OnError(ex);
					}

					// Signal to transmitter event, that writing
					// data to socket cannot be done
					m_connectedEvent->Reset();

					// Signal to dispatcher object, that IPC client was disconnected
					this->Invoke_OnDisconnected();

					// Add 50 seconds to reconnect
					if (waitTime < 1000)
					{
						waitTime += 50;
					}

				} while (m_terminateEvent->WaitOne(waitTime) == EventWaitHandle::WaitTimeout);
			}

			void IpcClient::TxThread()
			{
				do
				{
					// TODO Set wait to infinite, when queues are empty
					const auto waitResult = EventWaitHandle::WaitAny({ m_terminateEvent, m_queueChangedEvent }, 50);
					if (waitResult == EventWaitHandle::WaitTimeout)
					{
						this->DispatchExpiredMessages();
						this->DispatchExpiredRequests();
					}
					//
					// Received signal on queue changed event
					// send items from queue
					//
					else if (waitResult == EventWaitHandle::WaitObject<1>())
					{
						if (EventWaitHandle::WaitAny({ m_terminateEvent, m_connectedEvent }, 50) == EventWaitHandle::WaitObject<1>())
						{
							size_t prevQueueSize = 0;
							
							auto queueItem = this->PopQueueItem(prevQueueSize);
							if (queueItem)
							{
								auto success = false;

								auto socket = this->GetSocket();
								if (socket)
								{
									m_writeDoneEvent->Reset();

									try
									{
										const auto message = queueItem->Message();
										const auto timeout = queueItem->Timeout();

										this->WriteMessage(socket, message, timeout);

										// Message sent successfully
										if (queueItem->IsRequestItem())
										{
											auto requestQueueItem = std::dynamic_pointer_cast<details::IpcQueueRequestItem>(queueItem);

											{
												std::lock_guard<std::mutex> guard(m_requestsLock);

												m_requests[message->Id()] = requestQueueItem;
											}
										}
										else
										{
											auto responseQueueItem = dynamic_cast<details::IpcQueueResponseItem*>(queueItem.get());

											responseQueueItem->SetResult();
										}

										success = true;
									}
									catch (const std::exception& ex)
									{
										m_pDispatcher->IpcClient_OnError(ex);
									}

									m_writeDoneEvent->Set();
								}

								{
									std::lock_guard<std::mutex> guard(m_queueLock);

									// If sent was not successfull, place message to exactly same
									// queue position as before, to be processed in next iteration
									if (!success)
									{
										m_queue.insert(m_queue.begin() + (m_queue.size() - prevQueueSize), queueItem);
									}
									// Otherwise, if queue is already empty and all messages was sent successfully
									// we can reset changed event
									else if (m_queue.empty())
									{
										m_queueChangedEvent->Reset();
									}
								}
							}
						}
					}
					// Received signal on terminate event or wait failed. Exit loop
					else
					{
						break;
					}

				} while (1);

				this->CancelPendingMessages();
			}

			/**
			*/
			void IpcClient::Register(System::Net::Sockets::Socket_ptr socket)
			{
				std::string payload;

				payload.append(reinterpret_cast<const char*>(&m_clientId), sizeof(m_clientId));

				this->Invoke_EncyptPayload(payload);

				const auto message = IpcMessage::CreateClientRequest(IpcRegisterMessageId, payload);
				const auto timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

				this->WriteMessage(socket, message, timeout);
			}

			void IpcClient::WriteMessage(Socket_ptr socket, const IpcMessage_ptr message, const System::Timeout& timeout)
			{
				const auto& frame = message->Frame();

				socket->WriteAll(frame, timeout, m_terminateEvent);
			}

			// Read whole IPC message from socket
			IpcMessage_ptr IpcClient::ReadMessage()
			{
				do
				{
					if (!m_buffer.empty())
					{
						auto message = IpcMessage::PeekFromBuffer(m_buffer);
						if (message)
						{
							auto& payload = message->Payload();

							this->Invoke_DecryptPayload(payload);

							return message;
						}
					}

					const auto& data = m_socket->Read(m_terminateEvent);

					m_buffer.append(data);

				} while (true);
			}

			void IpcClient::DispatchExpiredMessages()
			{
				std::lock_guard<std::mutex> guard(m_queueLock);

				for (auto iter = m_queue.begin(); iter != m_queue.end(); /* DO NOT INCREMENT */)
				{
					const auto& queueItem = *iter;

					if (queueItem->IsExpired())
					{
						const auto pex = std::make_exception_ptr(TimeoutException());

						queueItem->SetResult(pex);

						iter = m_queue.erase(iter);
					}
					else
					{
						++iter;
					}
				}
			}

			void IpcClient::DispatchExpiredRequests()
			{
				std::lock_guard<std::mutex> guard(m_requestsLock);

				for (auto iter = m_requests.begin(); iter != m_requests.end(); /* DO NOT INCREMENT */)
				{
					const auto& requestQueueItem = iter->second;

					if (requestQueueItem->IsExpired())
					{
						const auto pex = std::make_exception_ptr(TimeoutException());

						requestQueueItem->SetResult(pex);

						iter = m_requests.erase(iter);
					}
					else
					{
						++iter;
					}
				}
			}

			void IpcClient::CancelPendingMessages()
			{
				std::lock_guard<std::mutex> guard(m_queueLock);

				for (auto& item : m_queue)
				{
					const auto pex = std::make_exception_ptr(OperationCanceledException());

					item->SetResult(pex);
				}

				m_queue.clear();
			}

			void IpcClient::ProcessResponse(const IpcMessage_ptr response)
			{
				details::IpcQueueRequestItem_ptr requestQueueItem;

				{
					std::lock_guard<std::mutex> guard(m_requestsLock);

					const auto iter = m_requests.find(response->Id());
					if (iter != m_requests.end())
					{
						requestQueueItem = iter->second;

						m_requests.erase(iter);
					}
				}

				if (requestQueueItem)
				{
					requestQueueItem->SetResult(response);
				}
				else
				{
					// TODO Received response but no request is awaiting in queue
				}
			}

			details::IpcQueueItem_ptr IpcClient::PopQueueItem(size_t& prevQueueSize)
			{
				details::IpcQueueItem_ptr queueItem;

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					if (!m_queue.empty())
					{
						queueItem = m_queue.front();

						m_queue.pop_front();

						prevQueueSize = m_queue.size();
					}
					else
					{
						prevQueueSize = 0;
					}
				}

				return queueItem;
			}

			Socket_ptr IpcClient::GetSocket()
			{
				std::lock_guard<std::mutex> guard(m_socketLock);

				return m_socket;
			}

			void IpcClient::SetSocket(System::Net::Sockets::Socket_ptr socket)
			{
				std::lock_guard<std::mutex> guard(m_socketLock);

				m_socket = socket;
			}

			void IpcClient::Invoke_OnConnected()
			{
				try
				{
					m_pDispatcher->IpcClient_OnConnected();
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcClient::Invoke_OnDisconnected()
			{
				try
				{
					m_pDispatcher->IpcClient_OnDisconnected();
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcClient::Invoke_OnMessage(const IpcMessage_ptr message)
			{
				try
				{
					m_pDispatcher->IpcClient_OnMessage(message);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcClient::Invoke_OnError(const std::exception& ex)
			{
				try
				{
					m_pDispatcher->IpcClient_OnError(ex);
				}
				catch (const std::exception &)
				{
					// TODO Log???
				}
			}

			void IpcClient::Invoke_EncyptPayload(std::string& payload)
			{
				try
				{
					m_pDispatcher->IpcClient_EncryptPayload(payload);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcClient::Invoke_DecryptPayload(std::string& payload)
			{
				try
				{
					m_pDispatcher->IpcClient_DecryptPayload(payload);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			/**
			*/
			void IpcClient::Start()
			{
				this->Stop();

				m_terminateEvent->Reset();

				m_rxThread = std::thread(&IpcClient::RxThread, this, m_host, m_port);
				m_txThread = std::thread(&IpcClient::TxThread, this);
			}

			/**
			*/
			void IpcClient::Stop()
			{
				m_terminateEvent->Set();

				if (m_txThread.joinable()) { m_txThread.join(); }
				if (m_rxThread.joinable()) { m_rxThread.join(); }
			}
		}
	}
}
