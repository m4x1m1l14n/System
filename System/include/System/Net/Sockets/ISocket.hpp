#ifndef __ISOCKET_HPP__
#define __ISOCKET_HPP__

#include <string>

#include <System/Timeout.hpp>
#include <System/Threading/ManualResetEvent.hpp>

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			class ISocket
			{
			public:
				virtual ~ISocket() { }

				virtual void Connect(const std::string& host, int port) = 0;
				virtual void Connect(const std::string& host, int port, const System::Timeout& timeout) = 0;
				virtual void Connect(const std::string& host, int port, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;
				virtual void Connect(const std::string& host, int port, const System::Timeout& timeout, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;

				virtual size_t Write(const std::string& data) = 0;
				virtual size_t Write(const std::string& data, const System::Timeout& timeout) = 0;
				virtual size_t Write(const std::string& data, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;
				virtual size_t Write(const std::string& data, const System::Timeout& timeout, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;
				virtual size_t Write(const void* data, size_t len, const System::Timeout& timeout, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;

				virtual std::string Read() = 0;
				virtual std::string Read(const System::Timeout& timeout) = 0;
				virtual std::string Read(const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;
				virtual std::string Read(const System::Timeout& timeout, const System::Threading::ManualResetEvent_ptr terminateEvent) = 0;
			};
		}
	}
}

#endif // __ISOCKET_HPP__
