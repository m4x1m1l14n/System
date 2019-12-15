#ifndef __WEBSOCKET_HPP__
#define __WEBSOCKET_HPP__

#include <WinSock2.h>

#include <System/Net/Sockets/ISocket.hpp>

#include <openssl/ssl.h>

// TODO Remove!
using namespace System::Threading;

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			const std::string WebSocketGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

			class WebSocket;

			typedef std::shared_ptr<WebSocket> WebSocket_ptr;

			class WebSocket
				: public System::Net::Sockets::ISocket
			{
			public:
				WebSocket();
				WebSocket(const std::shared_ptr<SSL_CTX> ctx);

				virtual void Connect(const std::string& host, int port, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override;
				virtual void Write(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent) override;
				virtual std::string Read(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override;

				// Chains
				virtual void Connect(const std::string& host, int port) override { return WebSocket::Connect(host, port, Timeout::Infinite); }
				virtual void Connect(const std::string& host, int port, const Timeout& timeout) override { return WebSocket::Connect(host, port, timeout, nullptr); }
				virtual void Connect(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent) override { return WebSocket::Connect(host, port, Timeout::Infinite, terminateEvent); }

				virtual void Write(const std::string& data) override { return WebSocket::Write(data, Timeout::Infinite); }
				virtual void Write(const std::string& data, const Timeout& timeout) override { return WebSocket::Write(data, timeout, nullptr); }
				virtual void Write(const std::string& data, const ManualResetEvent_ptr terminateEvent) override { return WebSocket::Write(data, Timeout::Infinite, terminateEvent); }
				virtual void Write(const std::string& data, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent) override { return WebSocket::Write(data.c_str(), data.length(), timeout, terminateEvent); }
				
				virtual std::string Read() override { return WebSocket::Read(Timeout::Infinite, nullptr); }
				virtual std::string Read(const Timeout& timeout) override { return WebSocket::Read(timeout, nullptr); }
				virtual std::string Read(const ManualResetEvent_ptr terminateEvent) override { return WebSocket::Read(Timeout::Infinite, terminateEvent); }

				virtual bool IsSecure() const { return false; }

				// TODO Protected when finished!
			public:
				static std::string GenerateSecWebsocketKey();
				static uint32_t GenerateMaskingKey();

			protected:
				std::string m_receiveBuffer;
				std::unique_ptr<ISocket> m_socket;
			};
		}
	}
}

#endif // __WEBSOCKET_HPP__
