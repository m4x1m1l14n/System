#ifndef __WEBSOCKET_CLIENT_HPP__
#define __WEBSOCKET_CLIENT_HPP__

#include <System/Net/WebSockets/IWebSocketClientDispatcher.hpp>
#include <System/Net/WebSockets/WebSocket.hpp>

#include <System/Threading/AutoResetEvent.hpp>

#include <openssl/ssl.h>
#include <iostream>
#include <deque>
#include <future>

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			namespace details
			{
				class SendQueueItem
				{
				public:
					SendQueueItem(const std::string& message, const Timeout& timeout, std::promise<void>& promise)
						: message(message)
						, timeout(timeout)
						, promise(promise)
					{

					}

				public:
					const std::string& message;
					const Timeout& timeout;
					std::promise<void>& promise;
				};
			}

			// TODO Inherit from TcpClient
			class WebSocketClient
			{
			public:
				WebSocketClient(const std::string& host, int port, IWebSocketClientDispatcher* dispatcher)
					: WebSocketClient(host, port, dispatcher, nullptr)
				{

				}

				WebSocketClient(const std::string& host, int port, IWebSocketClientDispatcher* dispatcher, std::shared_ptr<SSL_CTX> ctx)
					: m_host(host)
					, m_port(port)
					, m_ctx(ctx)
					, m_dispatcher(dispatcher)
					, m_terminateEvent(std::make_shared<ManualResetEvent>())
					, m_hasNewItemsEvent(std::make_shared<AutoResetEvent>())
					, m_canSendEvent(std::make_shared<ManualResetEvent>())
				{

				}

				virtual ~WebSocketClient()
				{
					this->Stop();
				}

				void Start()
				{
					this->Stop();

					m_terminateEvent->Reset();

					m_workerThread = std::thread(&WebSocketClient::WorkerThread, this);
				}

				void Stop()
				{
					m_terminateEvent->Set();

					if (m_workerThread.joinable())
					{
						m_workerThread.join();
					}
				}

				void Send(const std::string& message, const Timeout& timeout)
				{
					std::promise<void> promise;

					auto future = promise.get_future();

					auto item = details::SendQueueItem(message, timeout, promise);

					{
						std::lock_guard<std::mutex> guard(m_queueLock);

						m_queue.push_back(&item);
					}

					m_hasNewItemsEvent->Set();
					
					future.get();
				}

				void SenderThread()
				{
					while (EventWaitHandle::WaitAny({ m_terminateEvent, m_hasNewItemsEvent }) == 1)
					{
						decltype(m_queue) queue;

						{
							std::lock_guard<std::mutex> guard(m_queueLock);

							for (auto iter = m_queue.begin(); iter != m_queue.end(); /* DO NOT ICREMENT! */)
							{
								auto& item = *iter;

								if (item->timeout.GetIsElapsed())
								{
									iter = m_queue.erase(iter);

									item->promise.set_exception(
										// TODO Operation timed-out exception
										std::make_exception_ptr(
											std::runtime_error("Operation timed-out")
										)
									);
								}
								else
								{
									++iter;
								}
							}

							queue = m_queue;
							m_queue.clear();
						}

						if (EventWaitHandle::WaitAny({ m_terminateEvent, m_canSendEvent }) == 1)
						{
							for (const auto& item : queue)
							{
								auto& promise = item->promise;

								try
								{
									m_socket->Write(item->message, item->timeout);

									promise.set_value();
								}
								catch (const std::exception&)
								{
									promise.set_exception(
										std::current_exception()
									);
								}
							}
						}
					};
				}

				void WorkerThread()
				{
					auto sender = std::thread(&WebSocketClient::SenderThread, this);

					do
					{
						try
						{
							m_socket = std::make_unique<WebSocket>(m_ctx);

							const auto& timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

							m_socket->Connect(m_host, m_port/*, timeout*/);

							m_canSendEvent->Set();

							m_dispatcher->IWebSocketClient_OnConnected();
							
							do
							{
								const auto& data = m_socket->Read(m_terminateEvent);

								m_dispatcher->IWebSocketClient_OnMessage(data);

							} while (true);
						}
						catch (const std::exception& ex)
						{
							std::cout << ex.what() << std::endl;
						}

						m_canSendEvent->Reset();

						m_dispatcher->IWebSocketClient_OnDisconnected();

					} while (!m_terminateEvent->IsSet()); // TODO Iterative reconnect timeout

					sender.join();
				}

			protected:
				std::string m_host;
				int m_port;
				std::shared_ptr<SSL_CTX> m_ctx;

				std::deque<details::SendQueueItem*> m_queue;
				std::mutex m_queueLock;

				std::unique_ptr<WebSocket> m_socket;

				std::thread m_workerThread;
				ManualResetEvent_ptr m_terminateEvent;

				AutoResetEvent_ptr m_hasNewItemsEvent;
				ManualResetEvent_ptr m_canSendEvent;
				ManualResetEvent_ptr m_shouldSendEvent;

				IWebSocketClientDispatcher* m_dispatcher;
			};
		}
	}
}

#endif // !__WEBSOCKET_CLIENT_HPP__
