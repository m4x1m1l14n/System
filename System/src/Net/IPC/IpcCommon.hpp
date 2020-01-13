#ifndef __SYSTEM_NET_IPC_COMMON_INC__
#define __SYSTEM_NET_IPC_COMMON_INC__


#include <System/Net/IPC/IpcMessage.hpp>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			namespace details
			{
				/**
				* Parse and peek message from buffer of incoming data
				*
				* @param buffer	Non const reference to buffer containing data received from network stream.
				*				After successfull message parse, message data is trimed off from
				*				this buffer.
				* @param data	Non const reference to variable that will receive message payload
				* @return Whether complete message was present or not in provided input buffer
				* @warning Throws when buffer contains invalid data
				*/
				static bool PeekFrameFromBuffer(std::string& buffer, std::string& data)
				{
					// Check if buffer starts with message start indicator
					// if not, throw exception to remove this connection
					if (buffer.front() != IpcMessageStart)
					{
						throw std::runtime_error("Message in buffer not starting with message indicator");
					}

					// Check if underlying buffer holds at least whole message header
					if (buffer.size() <= IpcMessageHeaderSize)
					{
						return false;
					}

					// Parse message header length
					const std::uint32_t payloadLen = *reinterpret_cast<std::uint32_t*>(&buffer[1]);
					const std::uint32_t messageLen = IpcMessageHeaderSize + payloadLen;
					if (buffer.size() < messageLen)
					{
						return false;
					}

					data = buffer.substr(IpcMessageHeaderSize, payloadLen);

					buffer.erase(0, messageLen);

					return true;
				}

				static void ParseMessage(const std::string& frame, IpcMessageId& messageId, std::string& payload)
				{
					const auto minMessageLen = sizeof(IpcMessageId);

					if (frame.length() < minMessageLen)
					{
						throw std::runtime_error("Message does not meet required minimal length.");
					}

					messageId = *reinterpret_cast<const IpcMessageId*>(frame.data());
					payload = frame.substr(sizeof(IpcMessageId));
				}
			}
		}
	}
}

#endif // __SYSTEM_NET_IPC_COMMON_INC__
