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

				IpcMessage(const IpcMessageConfig config, const IpcMessageId id, const std::string& payload)
					: m_config(config)
					, m_id(id)
					, m_payload(payload)
				{

				}

				static IpcMessage_ptr CreateServerRequest(const IpcMessageId id, const std::string& payload)
				{
					const auto config = IpcServerMessageFlag | IpcRequestFlag;

					return std::make_shared<IpcMessage>(config, id, payload);
				}

				static IpcMessage_ptr CreateServerResponse(const IpcMessageId id, const std::string& payload)
				{
					const auto config = IpcServerMessageFlag | IpcResponseFlag;

					return std::make_shared<IpcMessage>(config, id, payload);
				}

				static IpcMessage_ptr CreateClientRequest(const IpcMessageId id, const std::string& payload)
				{
					const auto config = IpcClientMessageFlag | IpcRequestFlag;

					return std::make_shared<IpcMessage>(config, id, payload);
				}

				static IpcMessage_ptr CreateClientResponse(const IpcMessageId id, const std::string& payload)
				{
					const auto config = IpcClientMessageFlag | IpcResponseFlag;

					return std::make_shared<IpcMessage>(config, id, payload);
				}

				virtual ~IpcMessage()
				{

				}
 
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

					const auto payloadLen = *reinterpret_cast<const IpcMessageLength*>(&buffer[sizeof(IpcMessageStart)]);
					const auto messageLen = IpcMessageHeaderSize + payloadLen;
					if (buffer.size() < messageLen)
					{
						// We do not have enough data to parse whole frame
						return IpcMessage_ptr();
					}

					const auto config = static_cast<IpcMessageConfig>(buffer[sizeof(IpcMessageStart) + sizeof(IpcMessageLength)]);
					const auto id = *reinterpret_cast<const IpcMessageId*>(&buffer[sizeof(IpcMessageStart) + sizeof(payloadLen) + sizeof(config)]);
					const auto& payload = buffer.substr(sizeof(IpcMessageStart) + sizeof(payloadLen) + sizeof(config) + sizeof(id), payloadLen);

					const auto& frame = buffer.substr(0, messageLen);

					buffer.erase(0, messageLen);

					auto message = std::shared_ptr<IpcMessage>(new IpcMessage(config, id, payload, frame));

					return message;
				}

				std::string& Payload()
				{
					if (m_payload.empty())
					{
						// TODO Parse from frame
					}

					return m_payload;
				}

				const IpcMessageId Id() const
				{
					return m_id;
				}

				IpcMessageConfig Config() const
				{
					return m_config;
				}

				bool IsRequest() const
				{
					return m_config & IpcRequestFlag;
				}

				bool IsResponse() const
				{
					return !this->IsRequest();
				}

				bool IsServerMessage() const
				{
					return m_config & IpcServerMessageFlag;
				}

				bool IsClientMessage() const
				{
					return !this->IsServerMessage();
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
						frame.append(reinterpret_cast<const char*>(&m_config), sizeof(m_config));
						frame.append(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
						frame.append(m_payload);

						m_frame = std::move(frame);
					}

					return m_frame;
				}

			protected:
				IpcMessage(const IpcMessageConfig config, const IpcMessageId id, const std::string& payload, const std::string& frame)
					: m_config(config)
					, m_id(id)
					, m_payload(payload)
					, m_frame(frame)
				{

				}

			protected:
				IpcMessageId m_id;
				IpcMessageConfig m_config;

				std::string m_payload;
				std::string m_frame;
			};
		}
	}
}

#endif // __SYSTEM_NET_IPC_MESSAGE_HPP__
