#include <System/Net/IPC/IpcServer.hpp>

#include <System/Net/IPC/IpcResponse.hpp>

#include "IpcCommon.hpp"

#include <memory>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			namespace details
			{
				Socket_ptr CreateServerSocket(int port)
				{
					auto serverSocket = std::make_unique<Socket>();

					serverSocket->setBlocking(false);
					serverSocket->Bind("localhost", port);
					serverSocket->Listen(10);

					return serverSocket;
				}

				/**
				* 
				*/
				static IpcMessageId ParseFrame(const std::string& frame, std::string& message)
				{
					// TODO implement
					return 0;
				}

				static std::string CreateFrameFromMessage(const IpcMessage_ptr message)
				{
					const auto messageId = message->Id();
					const auto& payload = message->Payload();
					const auto payloadLen = static_cast<IpcMessageLength>(payload.length());

					std::string frame;

					const auto frameSize = IpcMessageHeaderSize + sizeof(messageId) + payloadLen;

					frame.reserve(frameSize);

					frame = IpcMessageStart;
					frame.append(reinterpret_cast<const char*>(&payloadLen), sizeof(payloadLen));
					frame.append(reinterpret_cast<const char*>(&messageId), sizeof(messageId));
					frame.append(payload);

					return frame;
				}

				inline timeval CreateTimeval(const System::TimeSpan& timeout)
				{
					const auto ms = timeout.GetTotalMilliseconds();

					timeval tv;

					tv.tv_sec = static_cast<long>(ms / 1000);
					tv.tv_usec = static_cast<long>(ms - (tv.tv_sec * 1000) * 1000);

					return tv;
				}

				void WriteMessage(Socket_ptr socket, const std::string & message, ManualResetEvent_ptr terminateEvent)
				{
					const auto length = static_cast<std::uint32_t>(message.length());

					std::string data;

					data.reserve(IpcMessageHeaderSize + message.length());

					data = IpcMessageStart;
					data.append(reinterpret_cast<const char*>(&length), sizeof(length));
					data.append(message);

					socket->Write(data, terminateEvent);
				}
			}

			std::atomic<std::uint32_t> IpcServer::s_messageIdIterator = 1;

			IpcServer::IpcServer(int port, IpcServerDispatcher * pDispatcher)
				: m_port(port)
				, m_pDispatcher(pDispatcher)
				, m_terminateEvent(std::make_shared<ManualResetEvent>())
			{
#if 0
				if (pDispatcher == nullptr)
				{
					throw std::invalid_argument("dispatcher cannot be null");
				}
#endif
			}

			/**
			* Class dtor
			*
			* Stops IPC server if not already stopped
			*/
			IpcServer::~IpcServer()
			{
				this->Stop();
			}

			void IpcServer::Start()
			{
				this->Stop();

				m_terminateEvent->Reset();

				m_acceptThread = std::thread(&IpcServer::AcceptThread, this, m_port);
				m_workerThread = std::thread(&IpcServer::WorkerThread, this);
			}

			void IpcServer::Stop()
			{
				m_terminateEvent->Set();

				if (m_acceptThread.joinable()) { m_acceptThread.join(); }
				if (m_workerThread.joinable()) { m_workerThread.join(); }
			}

			IpcRequest_ptr IpcServer::CreateRequest(const std::string& data)
			{
				const auto id = this->GenerateRequestId();

				return std::make_shared<IpcRequest>(id, data);
			}

			IpcResponse_ptr IpcServer::CreateResponse(const IpcRequest_ptr request, const std::string& data)
			{
				const auto id = request->Id() + 1;
				const auto response = std::make_shared<IpcResponse>(id, data);

				return response;
			}

			std::string IpcServer::SendRequest(const IpcClientId clientId, const IpcRequest_ptr request)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client is stopped");
				}

				{
					std::lock_guard<std::mutex> guard(m_clientsLock);

					auto iter = m_clients.find(clientId);
					if (iter == m_clients.end())
					{
						throw std::invalid_argument("ClientId \"" + std::to_string(clientId) + "\" not found!");
					}
					else
					{
						const auto& client = iter->second;

						client->txQueue.push_back(request);
					}
				}

				return request->Wait();
			}

			void IpcServer::SendResponse(const IpcClientId clientId, const IpcResponse_ptr response)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				{
					std::lock_guard<std::mutex> guard(m_clientsLock);

					auto iter = m_clients.find(clientId);
					if (iter == m_clients.end())
					{
						throw std::invalid_argument("ClientId \"" + std::to_string(clientId) + "\" not found!");
					}
					else
					{
						const auto& client = iter->second;

						client->txQueue.push_back(response);
					}
				}

				response->Wait();
			}

			/**
			*	Generates unique server request id
			*
			*	Request id consists of IPC server request flag, which is MSB set to 1. Whole 32 bit client Id
			*	and 31 bits of message unique iterator
			*/
			inline IpcMessageId IpcServer::GenerateRequestId()
			{
				return IpcServerRequestFlag | (s_messageIdIterator++ & 0x7fffffff);
			}


			void IpcServer::AcceptThread(int port)
			{
#if defined(_WIN32)
				// NOTE
				//	On Windows platform we can create timeval struct as const at the beginning
				//	of this function, because select API does not modify this struct internally
				//	instead of unix calls
				const timeval tv = details::CreateTimeval(TimeSpan::FromMilliseconds(100));
#endif // _WIN32

				int waitTime = 0;

				fd_set readfds, writefds, errorfds;

				std::map<SOCKET, details::Client_ptr> pendingClients;

				do
				{
					try
					{
						auto serverSocket = details::CreateServerSocket(port);

						auto sock = static_cast<SOCKET>(*serverSocket);

						auto maxfds = static_cast<int>(sock) + 1;

						// Handle exceptions from caller code
						try
						{
							m_pDispatcher->IpcServer_Opened();
						}
						catch (const std::exception&) {}

						do
						{
							// Close all timeouted registrations
							for (auto iter = pendingClients.begin(); iter != pendingClients.end(); /* DO NOT INCREMENT */)
							{
								auto& client = iter->second;

								if (client->timeout.GetIsElapsed())
								{
									iter = pendingClients.erase(iter);
								}
								else
								{
									++iter;
								}
							}

							// Cleanup fd sets
							FD_ZERO(&readfds);
							FD_ZERO(&writefds);
							FD_ZERO(&errorfds);

							// Add server socket to read & error fd set only
							// because no write to server socket is required
							FD_SET(sock, &readfds);
							FD_SET(sock, &errorfds);

							// Add all pending client sockets
							for (const auto& iter : pendingClients)
							{
								const auto& socket = iter.first;

								FD_SET(socket, &readfds);
								FD_SET(socket, &errorfds);
							}

#if !defined(_WIN32)
							timeval tv = CreateTimeval(TimeSpan::FromMilliseconds(100));
#endif // _WIN32

							// Wait for socket changes
							int err = ::select(maxfds, &readfds, &writefds, &errorfds, &tv);
							if (err == 0)
							{
								continue;
							}

							if (err == SOCKET_ERROR)
							{
								throw SocketException(::WSAGetLastError());
							}

							// Check errors on sockets
							if (errorfds.fd_count > 0)
							{
								auto count = errorfds.fd_count;

								// Handle errors on server socket
								if (FD_ISSET(static_cast<SOCKET>(*serverSocket), &errorfds))
								{
									throw SocketException(::WSAGetLastError());
								}

								// Handle client socket errors
								for (auto iter = pendingClients.begin(); iter != pendingClients.end(), count > 0; /* DO NOT INCREMENT */)
								{
									// If error on pending client, remove!
									if (FD_ISSET(iter->first, &errorfds))
									{
										iter = pendingClients.erase(iter);

										--count;
									}
									else
									{
										++iter;
									}
								}

								err -= errorfds.fd_count;
							}

							// Check data available on pending clients
							if (err > 0 && readfds.fd_count > 0)
							{
								auto count = readfds.fd_count;

								// Handle incoming connection request
								if (FD_ISSET(static_cast<SOCKET>(*serverSocket), &readfds))
								{
									try
									{
										auto clientSocket = serverSocket->Accept();

										auto pendingClient = new details::Client();

										pendingClient->socket = clientSocket;
										pendingClient->timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5000)); // TODO just debug

										pendingClients[static_cast<SOCKET>(*clientSocket)] = std::shared_ptr<details::Client>(pendingClient);
									}
									catch (const std::exception& ex)
									{
										std::cout << ex.what() << std::endl;
									}

									--count;
								}

								// Handle data from client sockets
								for (auto iter = pendingClients.begin(); iter != pendingClients.end(), count > 0; /* DO NOT INCREMENT */)
								{
									auto eraseClient = false;

									if (FD_ISSET(iter->first, &readfds))
									{
										try
										{
											auto& client = iter->second;
											auto& clientSocket = client->socket;

											const auto& data = clientSocket->ReadAvailable();

											assert(!data.empty());

											client->rxBuffer.append(data);

											std::string payload;

											auto message = IpcMessage::PeekFromBuffer(client->rxBuffer);
											if (message)
											{
												const auto& payload = message->Payload();
												// Payload during registration contains client-id, which is 32bit number
												if (payload.length() != 4)
												{
													throw std::runtime_error("Wrong registration payload!");
												}
												
												const auto clientId = *reinterpret_cast<const IpcClientId*>(payload.data());

												{
													// Enqueue client to worker thread
													std::lock_guard<std::mutex> guard(m_clientsLock);

													//
													// TODO merge in case client was connected before
													//
													m_clients[clientId] = client;
												}

												// Invoke client connected callback, but care for exceptions in user code
												try
												{
													m_pDispatcher->IpcServer_ClientConnected(clientId);
												}
												catch (const std::exception&) { }

												// We moved client after successfull registration
												// to connected clients, so we can erase it from pending
												// clients
												eraseClient = true;
											}
										}
										catch (const std::exception& ex)
										{
											eraseClient = true;

											std::cout << ex.what() << std::endl;
										}

										// Decrement count of processed socket events
										--count;
									}

									// Erase pending client due to error in processing registration
									// or just proceed to next client processing
									if (eraseClient)
									{
										iter = pendingClients.erase(iter);
									}
									else
									{
										++iter;
									}
								}

								// Decrement overall processed sockets changes
								err -= readfds.fd_count;
							}

						} while (!m_terminateEvent->IsSet());
					}
					catch (const std::exception& ex)
					{
						// TODO onerror dispatch
						std::cout << ex.what() << std::endl;
					}

					if (waitTime < 3000)
					{
						waitTime += 50;
					}

				} while (m_terminateEvent->WaitOne(waitTime) == EventWaitHandle::WaitTimeout);
			}


			void IpcServer::WorkerThread()
			{
#if defined(_WIN32)
				// NOTE
				//	On Windows platform we can create timeval struct as const at the beginning
				//	of this function, because select API does not modify this struct internally
				//	instead of unix calls
				const timeval tv = details::CreateTimeval(TimeSpan::FromMilliseconds(100));
#endif // _WIN32

				int waitTime = 0;

				fd_set readfds, writefds, errorfds;

				do
				{
					waitTime = 0;

					do
					{
						// TODO Remove expired messages to send

						// Wait for some clients to connect
						if (m_clients.empty())
						{
							std::this_thread::sleep_for(
								std::chrono::milliseconds(100)
							);

							continue;
						}

						// Empty fd sets
						FD_ZERO(&readfds);
						FD_ZERO(&writefds);
						FD_ZERO(&errorfds);

						// Populate fd sets with clients sockets
						{
							std::lock_guard<std::mutex> guard(m_clientsLock);

							for (const auto& iter : m_clients)
							{
								const auto& client = iter.second;

								const auto socket = static_cast<SOCKET>(*client->socket);

								FD_SET(socket, &readfds);
								FD_SET(socket, &errorfds);

								if (!client->txQueue.empty())
								{
									FD_SET(socket, &writefds);
								}
							}
						}

#if !defined(_WIN32)
						timeval tv = CreateTimeval(TimeSpan::FromMilliseconds(100));
#endif // _WIN32

						auto pwritefds = (writefds.fd_count)
							? &writefds
							: nullptr;

						int err = ::select(0, &readfds, pwritefds, &errorfds, &tv);
						if (err == 0)
						{
							continue;
						}

						if (err == SOCKET_ERROR)
						{
							const auto lastError = ::WSAGetLastError();

							throw SocketException(lastError);
						}

						{
							std::lock_guard<std::mutex> guard(m_clientsLock);

							// Check errors on sockets
							if (errorfds.fd_count > 0)
							{
								auto count = errorfds.fd_count;

								// Handle client socket errors
								for (auto iter = m_clients.begin(); iter != m_clients.end(), count > 0; /* DO NOT INCREMENT */)
								{
									// If error on pending client, remove!
									const auto& client = iter->second;

									if (FD_ISSET(static_cast<SOCKET>(*client->socket), &errorfds))
									{
										const auto clientId = iter->first;

										iter = m_clients.erase(iter);

										// NOTE
										//	Do not use reference to client after erasing
										//	Reference after this point is broken

										m_pDispatcher->IpcServer_ClientDisconnected(clientId);

										--count;
									}
									else
									{
										++iter;
									}
								}

								err -= errorfds.fd_count;
							}

							// Read available data from sockets
							if (err > 0 && readfds.fd_count > 0)
							{
								auto count = readfds.fd_count;

								for (auto iter = m_clients.begin(); iter != m_clients.end(), count > 0; /* DO NOT INCREMENT */)
								{
									auto erased = false;
									const auto& client = iter->second;

									if (FD_ISSET(static_cast<SOCKET>(*client->socket), &readfds))
									{
										try
										{
											const auto& data = client->socket->ReadAvailable();

											client->rxBuffer.append(data);

											auto message = IpcMessage::PeekFromBuffer(client->rxBuffer);
											if (message)
											{
												const auto& clientId = iter->first;

												std::shared_ptr<IpcRequest> request;

												{
													std::lock_guard<std::mutex> guard(m_requestsLock);

													auto iter = m_requests.find(message->Id());
													if (iter != m_requests.end())
													{
														request = iter->second;
													}
												}

												if (request)
												{
													request->SetResult(message->Payload());
												}
												else
												{
													m_pDispatcher->IpcServer_OnMessage(clientId, message->Id(), data);
												}
											}
										}
										catch (const std::exception& ex)
										{
											// TODO Separate method to remove client
											const auto& clientId = iter->first;

											// Remove client from clients map!
											iter = m_clients.erase(iter);

											erased = true;

											m_pDispatcher->IpcServer_ClientDisconnected(clientId);
										}

										--count;
									}

									if (!erased)
									{
										++iter;
									}
								}

								err -= readfds.fd_count;
							}

							// Write data where possible
							if (err > 0 && writefds.fd_count > 0)
							{
								auto count = writefds.fd_count;

								for (auto iter = m_clients.begin(); iter != m_clients.end(), count > 0; /* DO NOT INCREMENT */)
								{
									const auto& client = iter->second;

									if (FD_ISSET(static_cast<SOCKET>(*client->socket), &writefds))
									{
										if (client->txBuffer.empty())
										{
											// NOTE
											//	No other way should exists how we can get there
											//	Some messages must be present in tx queue
											assert(!client->txQueue.empty());

											const auto& message = client->txQueue.front();

											client->txBuffer = details::CreateFrameFromMessage(message);
										}

										// tx buffer here cannot be empty
										assert(!client->txBuffer.empty());

										try
										{
											// TODO Implement int Write(const std::string&) on socket class and use it here!
											// TODO Implement void WriteAll(const std::string&) to send all data
											client->socket->Write(client->txBuffer);

											auto written = client->txBuffer.length();
											client->txBuffer.clear();
											if (client->txBuffer.empty())
											{
												const auto& message = client->txQueue.front();

												if (message->IsRequest())
												{
													auto request = std::dynamic_pointer_cast<IpcRequest>(message);

													{
														std::lock_guard<std::mutex> guard(m_requestsLock);

														m_requests[message->Id()] = request;
													}
												}

												client->txQueue.pop_front();
											}
										}
										catch (const std::exception& ex)
										{
											m_pDispatcher->IpcServer_OnError(ex);

											// TODO Remove client
										}

										--count;
									}
								}

								err -= writefds.fd_count;
							}
						}

						// NOTE
						//	If err variable is not 0, we did not handle all situations on
						//	available sockets!
						assert(err == 0);

					} while (!m_terminateEvent->IsSet());

					waitTime = (waitTime > 3000)
						? 3000
						: waitTime += 50;

				} while (m_terminateEvent->WaitOne(waitTime) == EventWaitHandle::WaitTimeout);
			}

		}
	}
}
