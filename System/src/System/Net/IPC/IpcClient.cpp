#include <System/Net/IPC/IpcClient.hpp>

#include <iostream>

using namespace System::Threading;
using namespace System::Net::Sockets;

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			namespace internal
			{
				class ipcmessage
				{
					IpcMessageId id;
					std::string payload;
				};
			}

			std::atomic<std::uint32_t> IpcClient::s_instanceCounter = 1;
			std::atomic<std::uint32_t> IpcClient::s_messageIdIterator = 1;

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
				, m_pDispatcher(pDispatcher)
			{
				if (m_port == 0)
				{
					throw std::invalid_argument("port number must be spceified");
				}

				if (pDispatcher == nullptr)
				{
					throw std::invalid_argument("dispatcher cannot be null");
				}

				m_clientId = static_cast<uint64_t>(::GetCurrentProcessId()) << 32 | s_instanceCounter++;
			}

			/**
				\brief Class dtor

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

			std::string IpcClient::SendRequest(const std::string& data)
			{
				return this->SendRequest(data, TimeSpan::MaxValue());
			}

			std::string IpcClient::SendRequest(const std::string& data, const TimeSpan & timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				const auto id = this->GenerateRequestId(m_clientId);

				const auto& request = std::make_shared<IpcRequest>(id, data, timeout);

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					m_queue.push_back(request);

					m_queueChangedEvent->Set();
				}

				return request->Wait();
			}

			void IpcClient::SendResponse(const IpcMessageId messageId, const std::string& data)
			{
				return this->SendResponse(messageId, data, TimeSpan::MaxValue());
			}

			void IpcClient::SendResponse(const IpcMessageId messageId, const std::string& data, const TimeSpan & timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				const auto& response = std::make_shared<IpcResponse>(messageId, data, timeout);

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					m_queue.push_back(response);

					m_queueChangedEvent->Set();
				}

				response->Wait();
			}

			inline IpcMessageId IpcClient::GenerateRequestId(const IpcClientId clientId)
			{
				return static_cast<std::uint64_t>(clientId) << 31 | (s_messageIdIterator++ & 0x7fffffff);
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
						m_pDispatcher->IpcClient_OnConnected();

						// After successfull connect reset reconnect timeout to zero
						waitTime = 0;

						do
						{
							const auto& message = this->ReadMessage();

							// TODO Parse message ID from received message
							std::shared_ptr<IpcRequest> request;

							{
								std::lock_guard<std::mutex> guard(m_requestQueueLock);

								auto iter = m_requestQueue.find(id);
								if (iter != m_requestQueue.end())
								{
									request = iter->second;
								}
							}

							if (request)
							{
								request->SetResult(data);
							}
							else
							{
								m_pDispatcher->IpcClient_OnMessage(id, data);
							}

						} while (!m_terminateEvent->IsSet());
					}
					catch (const System::OperationCanceledException&)
					{
						// In case of operation cancelled, do not report error
					}
					catch (const std::exception& ex)
					{
						m_pDispatcher->IpcClient_OnError(ex.what());
					}

					// Signal to transmitter event, that writing
					// data to socket cannot be done
					m_connectedEvent->Reset();

					// Signal to dispatcher object, that IPC client was disconnected
					m_pDispatcher->IpcClient_OnDisconnected();

					// Add 50 seconds to reconnect
					if (waitTime < 1000)
					{
						waitTime += 50;
					}

				} while (m_terminateEvent->WaitOne(waitTime) == EventWaitHandle::WaitTimeout);

				std::cout << "Worker thread stopped" << std::endl;
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
							
							auto message = this->PopQueueMessage(prevQueueSize);
							if (message)
							{
								auto success = false;

								auto socket = this->GetSocket();
								if (socket)
								{
									m_writeDoneEvent->Reset();

									try
									{
										json11::Json json = json11::Json::object({
											{ "msg-id", message->Id() },
											{ "data", message->Data() }
										});

										const auto& data = json.dump();

										this->WriteMessage(socket, data);

										// Message sent successfully
										if (message->IsRequest())
										{
											auto request = std::dynamic_pointer_cast<IpcRequest>(message);

											{
												std::lock_guard<std::mutex> guard(m_requestQueueLock);

												m_requestQueue[request->Id()] = request;
											}
										}
										else
										{
											dynamic_cast<IpcResponse*>(message.get())->SetResult();
										}

										success = true;
									}
									catch (const std::exception& ex)
									{
										std::cout << ex.what() << std::endl;
									}

									m_writeDoneEvent->Set();
								}

								{
									std::lock_guard<std::mutex> guard(m_queueLock);

									// If sent was not successfull, place message to exactly same
									// queue position as before, to be processed in next iteration
									if (!success)
									{
										m_queue.insert(m_queue.begin() + (m_queue.size() - prevQueueSize), message);
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
				const json11::Json json = json11::Json::object({
					{ "msg-id", 0 },
					{ "client-id", static_cast<std::uint64_t>(m_clientId) }
				});

				const auto& requestData = json.dump();

				this->WriteMessage(socket, requestData);
			}

			void IpcClient::WriteMessage(Socket_ptr socket, const std::string & message)
			{
				const auto len = static_cast<std::uint32_t>(message.length());

				std::string data;

				data.reserve(IpcMessageHeaderSize + message.length());
				
				data = IpcMessageStart;
				data.append(reinterpret_cast<const char*>(&len), sizeof(len));
				data.append(message);

				socket->Write(data, m_terminateEvent);
			}

			// Read whole IPC message from socket
			std::string IpcClient::ReadMessage()
			{
				do
				{
					if (m_buffer.length() >= IpcMessageHeaderSize)
					{
						if (m_buffer[0] != 0xAA)
						{
							// TODO throw
						}

						const auto payloadLength = static_cast<std::uint32_t>(
							*reinterpret_cast<std::uint32_t*>(&m_buffer[1])
						);

						const auto messageLen = IpcMessageHeaderSize + payloadLength;

						if (m_buffer.length() >= messageLen)
						{
							const auto& message = m_buffer.substr(IpcMessageHeaderSize, payloadLength);

							m_buffer.erase(0, messageLen);

							return message;
						}
					}

					const auto& data = m_socket->Read(m_terminateEvent);

					m_buffer.append(data);

				} while (1);
			}

			void IpcClient::DispatchExpiredMessages()
			{
				std::lock_guard<std::mutex> guard(m_queueLock);

				for (auto iter = m_queue.begin(); iter != m_queue.end(); /* DO NOT INCREMENT */)
				{
					const auto& message = *iter;

					if (message->IsExpired())
					{
						message->SetResult(TimeoutException());

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
				std::lock_guard<std::mutex> guard(m_requestQueueLock);

				for (auto iter = m_requestQueue.begin(); iter != m_requestQueue.end(); /* DO NOT INCREMENT */)
				{
					const auto& request = iter->second;

					if (request->IsExpired())
					{
						request->SetResult(TimeoutException());

						iter = m_requestQueue.erase(iter);
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
					item->SetResult(OperationCanceledException());
				}

				m_queue.clear();
			}

			std::shared_ptr<IpcMessage> IpcClient::PopQueueMessage(size_t& prevQueueSize)
			{
				std::shared_ptr<IpcMessage> message;

				{
					std::lock_guard<std::mutex> guard(m_queueLock);

					if (!m_queue.empty())
					{
						message = m_queue.front();

						m_queue.pop_front();

						prevQueueSize = m_queue.size();
					}
					else
					{
						prevQueueSize = 0;
					}
				}

				return message;
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
