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
				HttpContent(const std::string& data, std::shared_ptr<System::Net::Sockets::ISocket> socket);

			private:
				std::shared_ptr<System::Net::Sockets::ISocket> m_socket;
				std::string m_buffer;
			};
		}
	}
}
