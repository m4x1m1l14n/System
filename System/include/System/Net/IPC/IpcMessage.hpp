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
			class IpcMessage
			{
			public:
				virtual ~IpcMessage()
				{

				}

				virtual void SetResult(const std::exception& ex) = 0;
				virtual bool IsRequest() const = 0;
				virtual bool IsResponse() const = 0;

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

					const auto dataLen = *reinterpret_cast<const IpcMessageLength*>(&buffer[1]);
					const auto messageLen = IpcMessageHeaderSize + dataLen;
					if (buffer.size() < messageLen)
					{
						// We do not have enough data to parse whole frame
						return IpcMessage_ptr();
					}

					const auto messageId = *reinterpret_cast<const IpcMessageId*>(&buffer[1 + sizeof(dataLen)]);

					const auto& data = buffer.substr(0, messageLen);

					buffer.erase(0, messageLen);
				}

				bool IsExpired() const
				{
					return m_timeout.GetIsElapsed();
				}

				const std::string& Payload() const
				{
					return m_payload;
				}

				const IpcMessageId Id() const
				{
					return m_id;
				}

				std::string Data()
				{
					if (m_data.empty())
					{
						m_data.reserve(IpcMessageHeaderSize + m_payload.length());

					}

					return m_data;
				}

			protected:
				IpcMessage(const IpcMessageId id, const std::string& payload, const Timeout& timeout)
					: m_id(id)
					, m_payload(payload)
					, m_timeout(timeout)
				{

				}

			protected:
				IpcMessageId m_id;
				std::string m_payload;
				System::Timeout m_timeout;
				std::string m_data;
			};

			typedef std::shared_ptr<IpcMessage> IpcMessage_ptr;
		}
	}
}

#endif // __SYSTEM_NET_IPC_MESSAGE_HPP__
