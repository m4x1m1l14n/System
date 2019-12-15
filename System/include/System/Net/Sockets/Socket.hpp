#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>

#include <System/Net/Sockets/ISocket.hpp>
#include <System/Net/Sockets/AddressFamily.hpp>
#include <System/Net/Sockets/ProtocolType.hpp>
#include <System/Net/Sockets/SocketType.hpp>

#include <System/Net/Sockets/SocketException.hpp>
#include <System/OperationCanceledException.hpp>

#include <System/Threading/ManualResetEvent.hpp>
#include <System/Timeout.hpp>

#ifdef _WIN32

#include <ppl.h>
#include <ppltasks.h>

#endif // _WIN32

// TODO Remove
using namespace System::Threading;

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			class Socket;

			typedef std::shared_ptr<Socket> Socket_ptr;

			class Socket
				: public ISocket
			{
			public:
				Socket();
				Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);

				virtual ~Socket();

				virtual void Connect(const std::string& host, int port);
				virtual void Connect(const std::string& host, int port, const Timeout& timeout);
				virtual void Connect(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent);
				virtual void Connect(const std::string& host, int port, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				concurrency::task<void> ConnectAsync(const std::string& host, int port);
				concurrency::task<void> ConnectAsync(const std::string& host, int port, const Timeout& timeout);
				concurrency::task<void> ConnectAsync(const std::string& host, int port, const ManualResetEvent_ptr terminateEvent);
				concurrency::task<void> ConnectAsync(const std::string& host, int port, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				void Bind(const std::string& host, int port);
				void Listen(int backlog);

				Socket_ptr Accept();

				virtual void Close();

				virtual size_t Write(const std::string& data);
				virtual size_t Write(const std::string& data, const Timeout& timeout);
				virtual size_t Write(const std::string& data, const ManualResetEvent_ptr terminateEvent);
				virtual size_t Write(const std::string& data, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);
				virtual size_t Write(const void* data, size_t len, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				void WriteAll(const std::string& data);
				void WriteAll(const std::string& data, const Timeout& timeout);
				void WriteAll(const std::string& data, const ManualResetEvent_ptr terminateEvent);
				void WriteAll(const std::string& data, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);
				void WriteAll(const void* data, size_t len, const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				concurrency::task<void> WriteAsync(const std::string& data);

				virtual std::string Read();
				virtual std::string Read(const Timeout& timeout);
				virtual std::string Read(const ManualResetEvent_ptr terminateEvent);
				virtual std::string Read(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				std::string ReadAvailable();

				concurrency::task<std::string> ReadAsync();
				concurrency::task<std::string> ReadAsync(const Timeout& timeout);
				concurrency::task<std::string> ReadAsync(const ManualResetEvent_ptr terminateEvent);
				concurrency::task<std::string> ReadAsync(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

				static void Select(std::vector<Socket_ptr>& read, std::vector<Socket_ptr>& write, std::vector<Socket_ptr>& error, const TimeSpan& timeout);

				explicit operator SOCKET() const;

				bool getBlocking() const;
				void setBlocking(bool blocking);

				std::string getHost() const;
				int getPort() const;

				__declspec(property(get = getBlocking, put = setBlocking)) bool Blocking;

				int GetSocketError() const;

			protected:
				Socket(SOCKET sock);

				void WaitReadReady(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);
				void WaitWriteReady(const Timeout& timeout, const ManualResetEvent_ptr terminateEvent);

			protected:
				SOCKET m_socket;

				AddressFamily m_addressFamily;
				SocketType m_socketType;
				ProtocolType m_protocolType;

				std::string m_host;
				int m_port;
			};
		}
	}
}
