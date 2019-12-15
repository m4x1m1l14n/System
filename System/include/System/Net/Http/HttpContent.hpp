#pragma once

#include <string>
#include <fstream>
#include <functional>

#include <System/Net/Sockets/Socket.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			class HttpContent
			{
			public:
				friend class HttpResponse;

				HttpContent();

				virtual std::string ReadAsString();
				virtual void ReadToFile(std::ofstream& out);
				virtual void Read(const std::function<bool(const std::string&)>& callback);

			private:
				HttpContent(const std::string& data);
				HttpContent(const std::string& data, System::Net::Sockets::Socket_ptr socket);

			private:
				System::Net::Sockets::Socket_ptr m_socket;
				std::string m_buffer;
			};
		}
	}
}
