#include <System/Net/WebSockets/WebSocket.hpp>

#include <System/Net/Sockets/Socket.hpp>
#include <System/Net/Sockets/TlsSocket.hpp>

#include <System/Net/WebSockets/WebSocketException.hpp>
#include <System/Net/WebSockets/WebSocketOpCode.hpp>
#include <System/Net/Http/HttpHeaders.hpp>

#include <Crypto/Random.hpp>
#include <Crypto/Base64.hpp>
#include <Crypto/Hash.hpp>

#include <sstream>

using namespace System::Net::Http;
using namespace System::Net::Sockets;

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			WebSocket::WebSocket()
				: WebSocket(nullptr)
			{
			}

			WebSocket::WebSocket(const std::shared_ptr<SSL_CTX> ctx)
			{
				m_socket = ctx
					? std::make_unique<TlsSocket>(ctx)
					: std::make_unique<Socket>();
			}

			void WebSocket::Connect(const std::string& host, int port, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				m_socket->Connect(host, port, timeout, terminateEvent);

				const auto& secWebSocketKey = WebSocket::GenerateSecWebsocketKey();

				// TODO Create using http headers class
				std::stringstream ss;

				ss
					<< "GET /chat HTTP/1.1\r\n"
					<< "Host: " << host << ":" << port << "\r\n"
					<< "Connection: Upgrade\r\n"
					<< "Upgrade: websocket\r\n"
					<< "Sec-WebSocket-Version: 13\r\n"
					<< "Sec-WebSocket-Key: " << secWebSocketKey << "\r\n"
					<< "\r\n";

				const auto& request = ss.str();

				m_socket->Write(request, timeout, terminateEvent);

				// TODO Receive full http header until \r\n
				const auto& response = m_socket->Read();

				// TODO Check valid HTTP header
				auto headers = HttpHeaders(response);

				const auto& secWebSocketAccept = headers["Sec-WebSocket-Accept"];

				const auto& calculatedAccept = Crypto::Base64::Encode(Crypto::Hash::Sha1(secWebSocketKey + WebSocketGuid));
				if (calculatedAccept != secWebSocketAccept)
				{
					// TODO Change to better exception
					throw WebSocketException(0, "Handshake failed");
				}
			}

			void WebSocket::Write(const void * data, size_t len, const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				const uint64_t max_frame_len = 0x4000; // max frame size 16K
				const unsigned char is_masked = 1;
				const unsigned char rsv1 = 0, rsv2 = 0, rsv3 = 0;
				unsigned char is_final;
				unsigned char frame_payload_data[max_frame_len];
				WebSocketOpCode opcode;

				const size_t max_frame_header_len = 2 + sizeof(uint32_t) + sizeof(uint64_t);
				unsigned char frame_header[max_frame_header_len];
				auto success = false;

				auto frame_count = static_cast<int>(std::ceil(len / static_cast<double>(max_frame_len)));

				for (decltype(frame_count) i = 0; i < frame_count; ++i)
				{
					const auto maskingKey = WebSocket::GenerateMaskingKey();

					success = false;
					opcode = (i == 0) ? WebSocketOpCode::text_frame : WebSocketOpCode::continuation_frame;

					is_final = (i == (frame_count - 1)) ? 1 : 0;

					auto frame_payload_len = is_final ? len % max_frame_len : max_frame_len;

					unsigned char *p = frame_header;
					*p++ = is_final << 7 | rsv1 << 6 | rsv2 << 5 | rsv3 << 4 | static_cast<unsigned char>(opcode);
					*p = is_masked << 7;

					if (frame_payload_len > 0xffff) {
						*p++ |= 127;

						for (int j = sizeof(uint64_t) - 1; j >= 0; --j) {
							*p++ = ((unsigned char*)&frame_payload_len)[j];
						}
					}
					else if (frame_payload_len > 125) {
						*p++ |= 126;

						for (int j = sizeof(uint16_t) - 1; j >= 0; --j) {
							*p++ = ((unsigned char*)&frame_payload_len)[j];
						}
					}
					else {
						*p++ |= frame_payload_len & 0x7f;
					}

					if (is_masked) {
						memcpy(p, &maskingKey, sizeof(maskingKey));
						p += sizeof(maskingKey);
					}

					m_socket->Write(reinterpret_cast<char*>(frame_header), static_cast<int>(p - frame_header), timeout, terminateEvent);

					memcpy(frame_payload_data, reinterpret_cast<const char*>(data) + (i * max_frame_len), static_cast<size_t>(frame_payload_len));

					if (is_masked) {
						p = frame_payload_data;

						for (int j = 0; p < (frame_payload_data + frame_payload_len); ++p, ++j) {
							*p = *p ^ ((unsigned char*)&maskingKey)[j % 4];
						}
					}

					m_socket->Write(reinterpret_cast<char*>(frame_payload_data), static_cast<int>(frame_payload_len), timeout, terminateEvent);
				}
			}

			std::string WebSocket::Read(const Timeout & timeout, const ManualResetEvent_ptr terminateEvent)
			{
				std::string ret;

				const int header_size = 2;

				/*uint64_t maskingKey = 0;*/
				unsigned char is_masked;
				unsigned char is_final, rsv1, rsv2, rsv3;
				WebSocketOpCode opcode = WebSocketOpCode::text_frame;
				size_t payload_len = 0;
				unsigned char payload_len_flag = 0;
				int rx_total = 0;
				int processed = 0;
				bool complete = false;

				rx_total = static_cast<int>(m_receiveBuffer.length());

				do
				{
					auto p = m_receiveBuffer.data() + processed;

					// ak sme prijali aspon jeden bajt a este sme ho nespracovali, poparsujeme prvy bajt hlavicky
					if (rx_total >= 1 && processed < 1)
					{
						++processed;

						is_final = *p & 0x80;
						rsv1 = *p & 0x40;
						rsv2 = *p & 0x20;
						rsv3 = *p & 0x10;
						opcode = (WebSocketOpCode)(*p & 0x0f);
#if defined(_DEBUG) && 0
						OutputDebugStringA(("OpCode: " + Crypto::Hex::Encode(&opcode, sizeof(opcode)) + "\n").c_str());
#endif
						++p;
					}

					// ak sme prijali aspon 2 bajty a este sme ich nespracovali, tak poparsujeme druhy bajt hlavicky
					if (rx_total >= 2)
					{
						if (processed < 2)
						{
							++processed;

							is_masked = *p & 0x80;
							payload_len_flag = *p & 0x7f;

							++p;
						}

						switch (opcode)
						{
						case WebSocketOpCode::text_frame:
						{
							// ak je payload len 126 potrebujeme mat data header + 2 bajty payload
							if (payload_len_flag == 126)
							{
								if (rx_total > header_size && processed < (header_size + sizeof(uint16_t)))
								{
									for (int i = std::min(static_cast<int>(sizeof(uint16_t)), rx_total - processed) - 1; i >= 0; --i)
									{
										((unsigned char*)&payload_len)[(header_size + sizeof(uint16_t)) - processed - 1] = *p++;
										++processed;
									}
								}
							}
							else if (payload_len_flag == 127)
							{
								if (rx_total > header_size && processed < (header_size + sizeof(uint64_t)))
								{
									for (int i = std::min(static_cast<int>(sizeof(uint64_t)), rx_total - processed) - 1; i >= 0; --i)
									{
										((unsigned char*)&payload_len)[(header_size + sizeof(uint64_t)) - processed - 1] = *p++;
										++processed;
									}
								}
							}
							else
							{
								payload_len = payload_len_flag;
							}

							if (payload_len)
							{
								int available = rx_total - processed;

								int cnt = std::min(static_cast<int>(payload_len), available);
								if (cnt)
								{
									ret.append(reinterpret_cast<const char*>(p), cnt);

									processed += cnt;

									payload_len -= cnt;
								}

								if (!payload_len)
								{
									complete = true;
								}
							}
						}
						break;

						case WebSocketOpCode::connection_close:
						default:
							complete = true;
							break;
						}
					}

					if (!complete)
					{
						try
						{
							const auto& data = m_socket->Read(timeout, terminateEvent);

							m_receiveBuffer.append(data);

							rx_total += static_cast<int>(data.length());
						}
						catch (const std::exception&)
						{
							m_receiveBuffer.clear();

							std::rethrow_exception(
								std::current_exception()
							);
						}
					}
					else
					{
						m_receiveBuffer.erase(0, processed);
					}

				} while (!complete);

				if (m_receiveBuffer.empty() && m_receiveBuffer.capacity() > 32 * 1024 * 1024)
				{
					m_receiveBuffer = std::string();
				}

				return ret;
			}

			std::string WebSocket::GenerateSecWebsocketKey()
			{
				return Crypto::Base64::Encode(
					Crypto::Random::GenerateArray(16)
				);
			}

			uint32_t WebSocket::GenerateMaskingKey()
			{
				return Crypto::Random::Generate<uint32_t>();
			}
		}
	}
}
