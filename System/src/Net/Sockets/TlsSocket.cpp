#include <System/Net/Sockets/TlsSocket.hpp>

#include <assert.h>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			TlsSocket::TlsSocket(const std::shared_ptr<SSL_CTX> ctx)
			{
				m_ssl = std::shared_ptr<SSL>(SSL_new(ctx.get()), [](SSL* ssl)
				{
					SSL_free(ssl);
				});
			}

			void TlsSocket::Connect(const std::string & host, int port, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{	
				Socket::Connect(host, port, timeout, terminateEvent);

				auto ssl = m_ssl.get();

				SSL_set_fd(ssl, static_cast<int>(m_socket));
				SSL_set_connect_state(ssl);

				do
				{
					auto tx = SSL_connect(ssl);
					auto err = SSL_get_error(ssl, tx);
					if (err == SSL_ERROR_NONE)
					{
						break;
					}
					else if (err == SSL_ERROR_WANT_READ)
					{
						this->WaitReadReady(timeout, terminateEvent);
					}
					else if (err == SSL_ERROR_WANT_WRITE)
					{
						assert(0);

						this->WaitWriteReady(timeout, terminateEvent);
					}
					else if (err == SSL_ERROR_SYSCALL)
					{
						throw SocketException(::WSAGetLastError(), "socket operation failed");
					}
					else
					{
						// TODO throw unhandled
						assert(0);
					}

				} while (true);
			}

			size_t TlsSocket::Write(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				auto ssl = m_ssl.get();

				int tx_total = 0;
				
				do
				{
					auto tx = SSL_write(ssl, reinterpret_cast<const char*>(data) + tx_total, static_cast<int>(len - tx_total));
					auto err = SSL_get_error(ssl, tx);
					if (err == SSL_ERROR_NONE)
					{
						tx_total += tx;
					}
					else if (err == SSL_ERROR_WANT_WRITE)
					{
						this->WaitWriteReady(timeout, terminateEvent);
					}
					else if (err == SSL_ERROR_SYSCALL)
					{
						throw SocketException(::WSAGetLastError(), "socket operation failed");
					}
					else
					{
						// TODO Throw unhandled code exception

						assert(0);
					}

				} while (tx_total < len);
			}

			std::string TlsSocket::Read(const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				auto ssl = m_ssl.get();

				char buffer[8 * 1024];

				do
				{
					auto rx = SSL_read(ssl, buffer, static_cast<int>(std::size(buffer)));
					int err = SSL_get_error(ssl, rx);
					if (err == SSL_ERROR_NONE)
					{
						return std::string(buffer, rx);
					}
					else if (err == SSL_ERROR_WANT_READ)
					{
						this->WaitReadReady(timeout, terminateEvent);
					}
					else if (err == SSL_ERROR_WANT_WRITE)
					{
						assert(0);

						this->WaitWriteReady(timeout, terminateEvent);
					}
					else if (err == SSL_ERROR_SYSCALL)
					{
						throw SocketException(::WSAGetLastError(), "socket operation failed");
					}
					else
					{
						// TODO Throw unhandled code exception

						assert(0);
					}

				} while (true);
			}
		}
	}
}
