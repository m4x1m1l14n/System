#include <System/Net/Http/HttpContent.hpp>

#include <System/Net/Http/HttpHeaders.hpp>
#include <System/NotImplementedException.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			HttpContent::HttpContent()
			{

			}

			std::string HttpContent::ReadAsString()
			{
				std::string result;

				this->Read([&result](const std::string& data) -> bool
				{
					result.append(data);

					return true;
				});

				return result;
			}

			void HttpContent::ReadToFile(std::ofstream & out)
			{
				this->Read([&out](const std::string& data) -> bool
				{
					out.write(data.c_str(), data.length());

					return true;
				});
			}

			void HttpContent::Read(const std::function<bool(const std::string&)>& callback)
			{
				auto stop = false;

				size_t blockLen;
				
				do
				{
					blockLen = 0;

					size_t pos;

					// Parse block length from incoming data
					do
					{
						pos = m_buffer.find(HttpHeaders::HeadersSeparator);
						if (pos == 0)
						{
							// Strip \r\n from the beggining of buffer
							m_buffer = m_buffer.substr(HttpHeaders::HeadersSeparator.length());
							pos = m_buffer.find(HttpHeaders::HeadersSeparator);
						}

						if (pos != std::string::npos)
						{
							blockLen = static_cast<size_t>(std::stoull(m_buffer.substr(0, pos), nullptr, 16));
							if (blockLen == 0)
							{
								stop = true;
							}
							else
							{
								m_buffer = m_buffer.substr(pos + HttpHeaders::HeadersSeparator.length());
							}
							
							break;
						}
						else
						{
							const auto& timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

							const auto& data = m_socket->Read(timeout);

							m_buffer.append(data);
						}
					} while (true);

					// Receive whole data chunk
					if (blockLen > 0)
					{
						if (!m_buffer.empty())
						{
							auto len = std::min(blockLen, m_buffer.length());

							stop = !callback(m_buffer.substr(0, len));

							blockLen -= len;

							m_buffer.erase(0, len);
						}

						if (blockLen > 0)
						{
							auto bytesToReceive = blockLen;

							do
							{
								const auto& timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

								const auto& data = m_socket->Read(timeout);

								auto len = std::min(data.length(), bytesToReceive);
								
								stop = !callback(data.substr(0, len));

								m_buffer.append(data.substr(len));

								bytesToReceive -= len;

							} while (bytesToReceive > 0);
						}
					}

				} while (!stop);
			}

			HttpContent::HttpContent(const std::string & data)
				: HttpContent(data, nullptr)
			{

			}

			HttpContent::HttpContent(const std::string & data, std::shared_ptr<System::Net::Sockets::ISocket> socket)
				: m_buffer(data)
				, m_socket(socket)
			{

			}
		}
	}
}
