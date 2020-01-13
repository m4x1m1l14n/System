#include <System/Net/Sockets/Socket.hpp>

#include <WS2tcpip.h>
#include <assert.h>

#include <sstream>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			Socket::Socket()
				: Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Unspecified)
			{
				
			}

			Socket::Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
				: m_addressFamily(addressFamily)
				, m_socketType(socketType)
				, m_protocolType(protocolType)
			{
				m_socket = ::socket(static_cast<int>(addressFamily), static_cast<int>(socketType), static_cast<int>(protocolType));
				if (m_socket == INVALID_SOCKET)
				{
					throw SocketException(::WSAGetLastError());
				}
			}

			Socket::~Socket()
			{
				this->Close();
			}

			void Socket::Connect(const std::string& host, int port)
			{
				return this->Connect(host, port, nullptr);
			}

			void Socket::Connect(const std::string& host, int port, const Timeout & timeout)
			{
				return this->Connect(host, port, timeout, nullptr);
			}

			void Socket::Connect(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent)
			{
				return this->Connect(host, port, Timeout::Infinite, terminateEvent);
			}

			void Socket::Connect(const std::string& host, int port, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent)
			{
				std::exception_ptr pex;

				// TODO endpoint lookup move to tcp client connect!
				SOCKET sock = INVALID_SOCKET;
				addrinfo *result = nullptr;

				try
				{
					addrinfo hints = { 0 };
					addrinfo *ptr = nullptr;

					ZeroMemory(&hints, sizeof(hints));

					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;

					int err = ::getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
					if (err != 0)
					{
						if (result != nullptr)
						{
							::freeaddrinfo(result);
						}

						throw SocketException(err, "getaddrinfo() for \"" + host + ":" + std::to_string(port) + "\" failed");
					}

					for (
						ptr = result;
						ptr != nullptr && (!terminateEvent || terminateEvent->IsSet());
						ptr = ptr->ai_next
						)
					{
						sock = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
						if (sock == INVALID_SOCKET)
						{
							continue;
						}

						unsigned long blocking = 1;
						err = ::ioctlsocket(sock, FIONBIO, &blocking);
						if (err != SOCKET_ERROR)
						{
							break;
						}
						else
						{
							::closesocket(sock);
							sock = INVALID_SOCKET;
						}
					}

					if (sock == INVALID_SOCKET)
					{
						throw SocketException(WSAECONNRESET, "Failed to create socket");
					}

					m_socket = sock;

					sockaddr_in local;
					local.sin_family = AF_INET;
					local.sin_addr.s_addr = INADDR_ANY;
					local.sin_port = 0; //randomly selected port

					err = ::bind(sock, reinterpret_cast<const sockaddr*>(&local), sizeof(local));
					if (err < 0)
					{
						throw SocketException(::WSAGetLastError(), "bind() on socket " + std::to_string(sock) + " failed");
					}
					else
					{
						err = ::connect(sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
						if (err == SOCKET_ERROR)
						{
							err = WSAGetLastError();
							if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
							{
								this->WaitWriteReady(timeout, terminateEvent);

								int optval = -1;
								socklen_t optlen = sizeof(optval);

								err = ::getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&optval), &optlen);
								if (err != SOCKET_ERROR)
								{
									if (optval == 0)
									{
										// Success, do nothing
									}
									else
									{
										throw SocketException(optval, "connect() failed");
									}
								}
								else
								{
									throw SocketException(err, "getsockopt() failed");
								}
							}
							else
							{
								throw SocketException(err, "connect() on socket " + std::to_string(sock) + " failed");
							}
						}
					}

					m_socket = sock;
				}
				catch (const std::exception&)
				{
					pex = std::current_exception();
				}

				if (result != nullptr)
				{
					freeaddrinfo(result);
				}

				if (pex)
				{
					if (sock != INVALID_SOCKET)
					{
						::closesocket(sock);
					}

					std::rethrow_exception(pex);
				}
			}

			void Socket::Bind(const std::string & host, int port)
			{
				UNREFERENCED_PARAMETER(host);

				sockaddr_in local = { 0 };
				local.sin_family = AF_INET;
				local.sin_addr.s_addr = INADDR_ANY;
				local.sin_port = ::htons(static_cast<u_short>(port));

				int err = ::bind(m_socket, reinterpret_cast<const sockaddr*>(&local), sizeof(local));
				if (err == SOCKET_ERROR)
				{
					throw SocketException(::WSAGetLastError());
				}
			}

			void Socket::Listen(int backlog)
			{
				int err = ::listen(m_socket, backlog);
				if (err == SOCKET_ERROR)
				{
					throw SocketException(::WSAGetLastError());
				}
			}

			Socket_ptr Socket::Accept()
			{
				auto sock = ::accept(m_socket, nullptr, nullptr);
				if (sock == INVALID_SOCKET)
				{
					throw SocketException(::WSAGetLastError());
				}

				// NOTE
				//	Cannot use std::make_shared, because compiler failed with
				//	error that it cannot access protected Socket ctor
				return std::shared_ptr<Socket>(new Socket(sock));
			}

			concurrency::task<void> Socket::ConnectAsync(const std::string& host, int port)
			{
				return ConnectAsync(host, port, Timeout::Infinite, nullptr);
			}

			concurrency::task<void> Socket::ConnectAsync(const std::string& host, int port, const Timeout & timeout)
			{
				return ConnectAsync(host, port, timeout, nullptr);
			}

			concurrency::task<void> Socket::ConnectAsync(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent)
			{
				return ConnectAsync(host, port, Timeout::Infinite, terminateEvent);
			}

			concurrency::task<void> Socket::ConnectAsync(const std::string& host, int port, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				return concurrency::create_task([this, host, port, timeout, terminateEvent]
				{
					this->Connect(host, port, timeout, terminateEvent);
				});
			}

			void Socket::Close()
			{
				if (m_socket != INVALID_SOCKET)
				{
					::closesocket(m_socket);

					m_socket = INVALID_SOCKET;
				}
			}

			size_t Socket::Write(const std::string& data)
			{
				return Socket::Write(data, Timeout::Infinite);
			}

			size_t Socket::Write(const std::string& data, const Timeout& timeout)
			{
				return Socket::Write(data, timeout, nullptr);
			}

			size_t Socket::Write(const std::string& data, const ManualResetEvent_ptr terminateEvent)
			{
				return Socket::Write(data, Timeout::Infinite, terminateEvent);
			}

			size_t Socket::Write(const std::string& data, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent)
			{
				return Socket::Write(data.c_str(), data.length(), timeout, terminateEvent);
			}

			size_t Socket::Write(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				auto tx = ::send(m_socket, reinterpret_cast<const char*>(data), static_cast<int>(len), 0);
				if (tx <= 0)
				{
					throw SocketException(::WSAGetLastError(), "send() returned " + std::to_string(tx));
				}

				return static_cast<size_t>(tx);
			}

			void Socket::WriteAll(const std::string & data)
			{
				return this->WriteAll(data.c_str(), data.length(), Timeout::CreateInfinite(), nullptr);
			}

			void Socket::WriteAll(const std::string & data, const Timeout & timeout)
			{
				return this->WriteAll(data.c_str(), data.length(), timeout, nullptr);
			}

			void Socket::WriteAll(const std::string & data, const ManualResetEvent_ptr terminateEvent)
			{
				return this->WriteAll(data.c_str(), data.length(), Timeout::CreateInfinite(), terminateEvent);
			}

			void Socket::WriteAll(const std::string & data, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				return this->WriteAll(data.c_str(), data.length(), timeout, terminateEvent);
			}

			void Socket::WriteAll(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				size_t tx_total = 0;

				do
				{
					this->WaitWriteReady(timeout, terminateEvent);

					const auto tx = this->Write(reinterpret_cast<const char*>(data) + tx_total, len - tx_total, timeout, terminateEvent);

					assert(tx != 0);

					tx_total -= tx;

				} while (tx_total < len);
			}

			concurrency::task<void> Socket::WriteAsync(const std::string & data)
			{
				return concurrency::create_task([this, data]
				{
					Write(data);
				});
			}

			std::string Socket::Read()
			{
				return Socket::Read(Timeout::Infinite, nullptr);
			}

			std::string Socket::Read(const Timeout& timeout)
			{
				return Socket::Read(timeout, nullptr);
			}

			std::string Socket::Read(const ManualResetEvent_ptr terminateEvent)
			{
				return Socket::Read(Timeout::Infinite, terminateEvent);
			}

			std::string Socket::Read(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent)
			{
				this->WaitReadReady(timeout, terminateEvent);

				return this->ReadAvailable();
			}

			std::string Socket::ReadAvailable()
			{
				char buffer[8 * 1024];

				int rx = ::recv(m_socket, buffer, ARRAYSIZE(buffer), 0);
				if (rx > 0)
				{
					return std::string(buffer, rx);
				}
				else
				{
					throw SocketException(::WSAGetLastError(), "recv() returned " + std::to_string(rx));
				}
			}

			concurrency::task<std::string> Socket::ReadAsync()
			{
				return ReadAsync(Timeout::Infinite, nullptr);
			}

			concurrency::task<std::string> Socket::ReadAsync(const Timeout& timeout)
			{
				return ReadAsync(timeout, nullptr);
			}

			concurrency::task<std::string> Socket::ReadAsync(const ManualResetEvent_ptr terminateEvent)
			{
				return ReadAsync(Timeout::Infinite, terminateEvent);
			}

			concurrency::task<std::string> Socket::ReadAsync(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent)
			{
				return concurrency::create_task([this, timeout, terminateEvent]() -> std::string
				{
					return this->Read(timeout, terminateEvent);
				});
			}

			void Socket::Select(std::vector<Socket_ptr>& read, std::vector<Socket_ptr>& write, std::vector<Socket_ptr>& error, const TimeSpan & timeout)
			{
				// TODO finish
				if (read.empty() && write.empty() && error.empty())
				{
					throw std::invalid_argument("At least one input set must contain at least one socket.");
				}

				int nfds = 0;

				fd_set readfds;
				fd_set writefds;
				fd_set errorfds;

				FD_ZERO(&readfds);
				FD_ZERO(&writefds);
				FD_ZERO(&errorfds);

				for (const auto& socket : read) { FD_SET(static_cast<SOCKET>(*socket), &readfds); }
				for (const auto& socket : write) { FD_SET(static_cast<SOCKET>(*socket), &writefds); }
				for (const auto& socket : error) { FD_SET(static_cast<SOCKET>(*socket), &errorfds); }

				auto readCopy = std::move(read);
				read.clear();

				auto writeCopy = std::move(write);
				write.clear();

				auto errorCopy = std::move(error);
				error.clear();

				const auto ms = timeout.GetTotalMilliseconds();

				timeval tv;

				tv.tv_sec = static_cast<long>(ms / 1000);
				tv.tv_usec = static_cast<long>(ms - (tv.tv_sec * 1000) * 1000);

				int result = ::select(nfds, &readfds, &writefds, &errorfds, &tv);
				if (result < 0)
				{
					throw SocketException(::WSAGetLastError());
				}
				else if (result > 0)
				{

				}
			}

			Socket::operator SOCKET() const
			{
				return m_socket;
			}

			bool Socket::getBlocking() const
			{
				return false;
			}

			void Socket::setBlocking(bool blocking)
			{
				unsigned long val = (blocking) ? 0 : 1;

				int err = ::ioctlsocket(m_socket, FIONBIO, &val);
				if (err == SOCKET_ERROR)
				{
					throw SocketException(::WSAGetLastError());
				}
			}

			std::string Socket::getHost() const
			{
				return m_host;
			}

			int Socket::getPort() const
			{
				return m_port;
			}

			int Socket::GetSocketError() const
			{
				int optval = -1;
				socklen_t optlen = sizeof(optval);

				auto err = ::getsockopt(m_socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&optval), &optlen);
				if (err == SOCKET_ERROR)
				{
					throw SocketException(err, "getsockopt() failed");
				}

				return optval;
			}

			Socket::Socket(SOCKET sock)
				: m_socket(sock)
			{

			}

			void Socket::WaitReadReady(const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				int ret = SOCKET_ERROR;
				fd_set readfds, errorfds;
				const int maxfd = static_cast<int>(m_socket) + 1;

				do
				{
					FD_ZERO(&readfds);
					FD_SET(m_socket, &readfds);

					FD_ZERO(&errorfds);
					FD_SET(m_socket, &errorfds);

					timeval tv = { 0, 100 * 1000 };

					ret = ::select(maxfd, &readfds, nullptr, &errorfds, &tv);
					if (ret > 0)
					{
						if (FD_ISSET(m_socket, &readfds))
						{
							break;
						}
						else if (FD_ISSET(m_socket, &errorfds))
						{
							// TODO Exception
							assert(0);
						}
						else
						{
							// TODO Exception
							assert(0);
						}
					}
					else if (ret < 0)
					{
						throw SocketException(::WSAGetLastError(), "select() returned SOCKET_ERROR");
					}

					timeout.ThrowIfElapsed();

					if (terminateEvent && terminateEvent->IsSet())
					{
						throw System::OperationCanceledException();
					}

				} while (ret == 0);
			}

			void Socket::WaitWriteReady(const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				int ret = SOCKET_ERROR;
				fd_set writefds, errorfds;
				const int maxfd = static_cast<int>(m_socket) + 1;

				do
				{
					FD_ZERO(&writefds);
					FD_SET(m_socket, &writefds);

					FD_ZERO(&errorfds);
					FD_SET(m_socket, &errorfds);

					timeval tv = { 0, 100 * 1000 };

					ret = ::select(maxfd, nullptr, &writefds, &errorfds, &tv);
					if (ret > 0)
					{
						if (FD_ISSET(m_socket, &writefds))
						{
							break;
						}
						else if (FD_ISSET(m_socket, &errorfds))
						{
							throw SocketException(this->GetSocketError());
						}
						else
						{
							// TODO throw
							assert(0);
						}

						break;
					}
					else if (ret < 0)
					{
						throw SocketException(::WSAGetLastError(), "select() returned " + std::to_string(ret));
					}

					timeout.ThrowIfElapsed();

					if (terminateEvent && terminateEvent->IsSet())
					{
						// TODO Throw OperationCanceledException
						throw std::runtime_error("operation canceled");
					}

				} while (ret == 0);
			}
		}
	}
}
