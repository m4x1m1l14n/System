#include <System/Net/IPC/IpcServer.hpp>

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

				inline timeval CreateTimeval(const System::TimeSpan& timeout)
				{
					const auto ms = timeout.GetTotalMilliseconds();

					timeval tv;

					tv.tv_sec = static_cast<long>(ms / 1000);
					tv.tv_usec = static_cast<long>(ms - (static_cast<double>(tv.tv_sec) * 1000) * 1000);

					return tv;
				}
			}


			std::atomic<std::uint32_t> IpcServer::s_messageIdIterator = 1;


			IpcServer::IpcServer(int port, IpcServerDispatcher * pDispatcher)
				: m_port(port)
				, m_pDispatcher(pDispatcher)
				, m_terminateEvent(std::make_shared<ManualResetEvent>())
			{
#if defined(_DEBUG) && 0
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


			int IpcServer::Port() const
			{
				return m_port;
			}


			IpcMessage_ptr IpcServer::CreateRequest(const std::string& data)
			{
				const auto id = this->GenerateRequestId();
				const auto request = IpcMessage::CreateServerRequest(id, data);

				return request;
			}


			IpcMessage_ptr IpcServer::CreateResponse(const IpcMessage_ptr request, const std::string& data)
			{
				const auto id = request->Id();
				const auto response = IpcMessage::CreateServerResponse(id, data);

				return response;
			}


			IpcMessage_ptr IpcServer::SendRequest(const IpcClientId clientId, const IpcMessage_ptr request)
			{
				return this->SendRequest(clientId, request, Timeout::Infinite);
			}


			/** Sends request to specified client
			 * 
			 * @param[in] clientId ID of client to whom the request is addressed
			 * @param[in] request The request object
			 * @param[in] timeout Timeout object to specify amount of time, which server will wait for response
			 * @return IPC client response
			 *
			 * @note This method does not send request directly. Only enqueue request and waits for response
			 * @throw std::runtime_error When IPC server was not started yet
			 * @throw std::invalid_argument When IPC client with provided client ID was not connected yet
			 * @throw System::TimeoutException In case client does not respond to request within specified timeout
			 */
			IpcMessage_ptr IpcServer::SendRequest(const IpcClientId clientId, const IpcMessage_ptr request, const System::Timeout& timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client is stopped");
				}

				details::IpcQueueRequestItem_ptr requestQueueItem;

				{
					std::lock_guard<std::mutex> guard(m_clientsLock);

					auto iter = m_clients.find(clientId);
					if (iter == m_clients.end())
					{
						throw std::invalid_argument("IPC client with ID " + std::to_string(clientId) + " not found!");
					}
					else
					{
						const auto& client = iter->second;

						requestQueueItem = std::make_shared<details::IpcQueueRequestItem>(request, timeout);

						client->txQueue.push_back(requestQueueItem);
					}
				}

				// At this point, request queue item cannot be unset
				assert(requestQueueItem);

				return requestQueueItem->Wait();
			}


			void IpcServer::SendResponse(const IpcClientId clientId, const IpcMessage_ptr response)
			{
				return this->SendResponse(clientId, response, Timeout::Infinite);
			}


			void IpcServer::SendResponse(const IpcClientId clientId, const IpcMessage_ptr response, const System::Timeout& timeout)
			{
				if (m_terminateEvent->IsSet())
				{
					throw std::runtime_error("IPC client stopped");
				}

				details::IpcQueueResponseItem_ptr responseQueueItem;

				{
					std::lock_guard<std::mutex> guard(m_clientsLock);

					auto iter = m_clients.find(clientId);
					if (iter == m_clients.end())
					{
						throw std::invalid_argument("IPC client with ID " + std::to_string(clientId) + " not found!");
					}
					else
					{
						const auto& client = iter->second;

						responseQueueItem = std::make_shared<details::IpcQueueResponseItem>(response, timeout);

						client->txQueue.push_back(responseQueueItem);
					}
				}

				// At this point response queue item cannot be empty
				assert(responseQueueItem);

				responseQueueItem->Wait();
			}


			/**
			 * Generates unique server IPC request ID number
			 *
			 * Request ID consists of IPC server request flag (which is MSB set to 1)
			 * and per instance unique message ID number.
			 *
			 *  _________________________
			 * |   63  |     62 - 0      |
			 * |-------------------------|
			 * |   1   | request number	 |
			 * |-------------------------|
			 *
			 * @return Unique IPC server request ID
			 */
			inline IpcMessageId IpcServer::GenerateRequestId()
			{
				return s_messageIdIterator++;
			}

			/**
			 * Removes clients from map, which was connected successfully connected on network layer,
			 * but did not sent registration message within specified timeout and thus their
			 * registration is considered expired.
			 */
			void RemoveClientsWithExpiredRegistration(std::map<SOCKET, details::Client_ptr>& clients)
			{
				if (clients.empty())
				{
					return;
				}

				for (auto iter = clients.begin(); iter != clients.end(); /* DO NOT INCREMENT */)
				{
					const auto& client = iter->second;
					const auto& timeout = client->timeout;

					if (timeout.GetIsElapsed())
					{
						iter = clients.erase(iter);
					}
					else
					{
						++iter;
					}
				}
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

						this->Invoke_Opened();

						do
						{
							RemoveClientsWithExpiredRegistration(pendingClients);

							// Cleanup fd sets
							FD_ZERO(&readfds);
							FD_ZERO(&writefds);
							FD_ZERO(&errorfds);

							// Add server socket only to read & error set,
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

							// We cannot recover from error on select API
							// throw to leave looping
							if (err == SOCKET_ERROR)
							{
								const auto lastError = ::WSAGetLastError();

								throw SocketException(lastError);
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
								for (auto iter = pendingClients.begin(); iter != pendingClients.end() && count > 0; /* DO NOT INCREMENT */)
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
										pendingClient->timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

										pendingClients[static_cast<SOCKET>(*clientSocket)] = std::shared_ptr<details::Client>(pendingClient);
									}
									catch (const std::exception& ex)
									{
										this->Invoke_OnError(ex);
									}

									--count;
								}

								// Handle data from client sockets
								for (auto iter = pendingClients.begin(); iter != pendingClients.end() && count > 0; /* DO NOT INCREMENT */)
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

											auto message = IpcMessage::PeekFromBuffer(client->rxBuffer);
											if (message)
											{
												auto payload = message->Payload();

												this->Invoke_DecryptPayload(payload);

												// Payload during registration contains client-id only
												if (message->Id() != IpcRegisterMessageId || payload.length() != sizeof(IpcClientId))
												{
													throw std::runtime_error("Wrong registration payload!");
												}
												
												const auto clientId = *reinterpret_cast<const IpcClientId*>(payload.data());
												client->clientId = clientId;

												{
													// Enqueue client to worker thread
													std::lock_guard<std::mutex> guard(m_clientsLock);

													//
													// TODO merge in case client was connected before
													//
													m_clients[clientId] = client;
												}

												this->Invoke_ClientConnected(clientId);

												// Dispatch all received messages
												while (!client->rxBuffer.empty())
												{
													message = IpcMessage::PeekFromBuffer(client->rxBuffer);
													if (message)
													{
														this->Invoke_OnMessage(clientId, message);
													}
													else
													{
														break;
													}
												}

												// We moved client after successfull registration
												// to connected clients, so we can erase it from pending
												// clients
												eraseClient = true;
											}
										}
										catch (const std::exception& ex)
										{
											eraseClient = true;

											this->Invoke_OnError(ex);
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
						this->Invoke_OnError(ex);
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
								for (auto iter = m_clients.begin(); iter != m_clients.end() && count > 0; /* DO NOT INCREMENT */)
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

								for (auto iter = m_clients.begin(); iter != m_clients.end() && count > 0; /* DO NOT INCREMENT */)
								{
									auto erased = false;
									const auto& client = iter->second;

									if (FD_ISSET(static_cast<SOCKET>(*client->socket), &readfds))
									{
										try
										{
											const auto& data = client->socket->ReadAvailable();

											client->rxBuffer.append(data);

											do
											{
												auto message = IpcMessage::PeekFromBuffer(client->rxBuffer);
												if (message)
												{
													if (message->IsResponse())
													{
														this->ProcessResponse(message);
													}
													else
													{
														const auto& clientId = iter->first;

														this->Invoke_OnMessage(clientId, message);
													}
												}
												else
												{
													break;
												}

											} while (!client->rxBuffer.empty());
											
										}
										catch (const std::exception&)
										{
											// TODO Separate method to remove client
											const auto clientId = iter->first;

											// Remove client from clients map!
											iter = m_clients.erase(iter);

											erased = true;
											
											this->Invoke_ClientDisconnected(clientId);
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

								for (auto iter = m_clients.begin(); iter != m_clients.end() && count > 0; ++iter)
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

											const auto& queueItem = client->txQueue.front();
											const auto& message = queueItem->Message();

											client->txBuffer = message->Frame();
										}

										// tx buffer here cannot be empty
										assert(!client->txBuffer.empty());

										try
										{
											const auto written = client->socket->Write(client->txBuffer);

											// Strip written bytes from tx buffer
											client->txBuffer.erase(0, written);

											// If whole message was sent, post-process message
											if (client->txBuffer.empty())
											{
												const auto& queueItem = client->txQueue.front();

												if (queueItem->IsRequestItem())
												{
													const auto requestQueueItem = std::dynamic_pointer_cast<details::IpcQueueRequestItem>(queueItem);
													const auto& request = requestQueueItem->Message();

													{
														std::lock_guard<std::mutex> guard(m_requestsLock);

														m_requests[request->Id()] = requestQueueItem;
													}
												}
												else
												{
													const auto responseQueueItem = std::dynamic_pointer_cast<details::IpcQueueResponseItem>(queueItem);

													responseQueueItem->SetResult();
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


			void IpcServer::ProcessResponse(const IpcMessage_ptr response)
			{
				details::IpcQueueRequestItem_ptr requestQueueItem;

				{
					std::lock_guard<std::mutex> guard(m_requestsLock);

					auto iter = m_requests.find(response->Id());
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


			void IpcServer::Invoke_Opened()
			{
				try
				{
					m_pDispatcher->IpcServer_Opened();
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_ClientConnected(const IpcClientId clientId)
			{
				try
				{
					m_pDispatcher->IpcServer_ClientConnected(clientId);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_ClientDisconnected(const IpcClientId clientId)
			{
				try
				{
					m_pDispatcher->IpcServer_ClientDisconnected(clientId);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_OnMessage(const IpcClientId clientId, const IpcMessage_ptr message)
			{
				try
				{
					m_pDispatcher->IpcServer_OnMessage(clientId, message);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_EncryptPayload(std::string& payload)
			{
				try
				{
					m_pDispatcher->IpcServer_EncryptPayload(payload);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_DecryptPayload(std::string& payload)
			{
				try
				{
					m_pDispatcher->IpcServer_DecryptPayload(payload);
				}
				catch (const std::exception & ex)
				{
					this->Invoke_OnError(ex);
				}
			}

			void IpcServer::Invoke_OnError(const std::exception& ex)
			{
				try
				{
					m_pDispatcher->IpcServer_OnError(ex);
				}
				catch (const std::exception &) { }
			}

		}
	}
}
