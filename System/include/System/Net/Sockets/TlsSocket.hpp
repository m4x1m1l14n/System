#ifndef __TLS_SOCKET_HPP__
#define __TLS_SOCKET_HPP__

#include <System/Net/Sockets/Socket.hpp>

#include <openssl/ssl.h>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			class TlsSocket
				: public Socket
			{
			public:
				TlsSocket(const std::shared_ptr<SSL_CTX> ctx);

				virtual void Connect(const std::string& host, int port, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override;
				virtual size_t Write(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent) override;
				virtual std::string Read(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override;

				// Chain to Connect method
				// TODO
				// virtual void Connect(const System::Net::Url& url) override { return TlsSocket::Connect(url.getHost(), url.getPort()); }
				virtual void Connect(const std::string& host, int port) override { return TlsSocket::Connect(host, port, Timeout::Infinite); }
				virtual void Connect(const std::string& host, int port, const Timeout& timeout) override { return TlsSocket::Connect(host, port, timeout, nullptr); }
				virtual void Connect(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent) override { return TlsSocket::Connect(host, port, Timeout::Infinite, terminateEvent); }
				
				// Chain to Write method
				virtual size_t Write(const std::string& data) override { return TlsSocket::Write(data, Timeout::Infinite); }
				virtual size_t Write(const std::string& data, const Timeout& timeout) override { return TlsSocket::Write(data, timeout, nullptr); }
				virtual size_t Write(const std::string& data, const ManualResetEvent_ptr terminateEvent) override { return TlsSocket::Write(data, Timeout::Infinite, terminateEvent); }
				virtual size_t Write(const std::string& data, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override { return TlsSocket::Write(data.c_str(), data.length(), timeout, terminateEvent); }

				// Chain for Read method
				virtual std::string Read() override { return TlsSocket::Read(Timeout::Infinite); }
				virtual std::string Read(const Timeout& timeout) override { return TlsSocket::Read(timeout, nullptr); }
				virtual std::string Read(const ManualResetEvent_ptr terminateEvent) override { return TlsSocket::Read(Timeout::Infinite, terminateEvent); }

			protected:
				std::shared_ptr<SSL> m_ssl;
			};
		}
	}
}

#endif // __TLS_SOCKET_HPP__
