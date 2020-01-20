#ifndef __SYSTEM_NET_IPC_MESSAGE_HPP__
#define __SYSTEM_NET_IPC_MESSAGE_HPP__

#include <System/Timeout.hpp>
#include <System/TimeSpan.hpp>
#include <System/TimeoutException.hpp>

#include <System/Net/IPC/IpcDefines.hpp>

#include <future>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			// Forward declaration for typedef
			class IpcMessage;

			typedef std::shared_ptr<IpcMessage> IpcMessage_ptr;

			class IpcMessage
			{
			public:
				IpcMessage(const std::string& frame)
					: m_frame(frame)
				{
					// TODO lazy payload parsing!
				}

				IpcMessage(const IpcMessageId id, const std::string& payload)
					: m_id(id)
					, m_payload(payload)
				{

				}

				virtual ~IpcMessage()
				{

				}

				// TODO When parsing message from buffer, store frame 
				static IpcMessage_ptr PeekFromBuffer(std::string& buffer)
				{
					// Check if buffer starts with frame start indicator
					// if not, throw exception to remove this connection
					if (buffer.front() != IpcMessageStart)
					{
						throw std::runtime_error("Frame in buffer not starting with correct indicator");
					}

					// Check if underlying buffer holds at least whole message header
					if (buffer.size() < IpcMessageHeaderSize)
					{
						return IpcMessage_ptr();
					}

					const auto payloadLen = *reinterpret_cast<const IpcMessageLength*>(&buffer[1]);
					const auto messageLen = IpcMessageHeaderSize + payloadLen;
					if (buffer.size() < messageLen)
					{
						// We do not have enough data to parse whole frame
						return IpcMessage_ptr();
					}

					const auto id = *reinterpret_cast<const IpcMessageId*>(&buffer[1 + sizeof(payloadLen)]);
					const auto& payload = buffer.substr(1 + sizeof(payloadLen) + sizeof(id), payloadLen);

					buffer.erase(0, messageLen);

					auto message = std::make_shared<IpcMessage>(id, payload);

					return message;
				}

				std::string& Payload()
				{
					return m_payload;
				}

				const IpcMessageId Id() const
				{
					return m_id;
				}

				std::string Frame()
				{
					if (m_frame.empty())
					{
						const auto payloadLen = static_cast<IpcMessageLength>(m_payload.length());

						std::string frame;

						const auto frameSize = IpcMessageHeaderSize + payloadLen;

						frame.reserve(frameSize);

						frame = IpcMessageStart;
						frame.append(reinterpret_cast<const char*>(&payloadLen), sizeof(payloadLen));
						frame.append(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
						frame.append(m_payload);

						m_frame = std::move(frame);
					}

					return m_frame;
				}

			protected:
				IpcMessageId m_id;
				std::string m_payload;
				std::string m_frame;
			};
		}
	}
}

#endif // __SYSTEM_NET_IPC_MESSAGE_HPP__
