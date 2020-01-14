#ifndef __SYSTEM_NET_IPC_FRAME_HPP__
#define __SYSTEM_NET_IPC_FRAME_HPP__


#include <System/Net/IPC/IpcMessage.hpp>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcFrame
			{
			public:
				/**
				* Default Ctor
				*
				* Constructs empty IPC frame
				*/
				IpcFrame()
				{

				}

				/**
				* Ctor
				*
				* Constructs frame from IPC message object
				*/
				IpcFrame(const IpcMessage_ptr message)
				{
					const auto id = message->Id();
					const auto& payload = message->Payload();
					const auto payloadLen = static_cast<IpcMessageLength>(payload.length());

					std::string data;

					data.reserve(IpcMessageHeaderSize + payloadLen);

					data = IpcMessageStart;
					data.append(reinterpret_cast<const char*>(&payloadLen), sizeof(payloadLen));
					data.append(reinterpret_cast<const char*>(&id), sizeof(id));
					data.append(payload);

					m_data = data;
				}

				static IpcFrame_ptr PeekFromBuffer(std::string& buffer)
				{
					// Check if buffer starts with frame start indicator
					// if not, throw exception to remove this connection
					if (buffer.front() != IpcFrameStart)
					{
						throw std::runtime_error("Frame in buffer not starting with correct indicator");
					}

					// Check if underlying buffer holds at least whole message header
					if (buffer.size() < IpcFrameHeaderSize)
					{
						return IpcFrame_ptr();
					}

					const auto payloadLen = *reinterpret_cast<const IpcFrameLength*>(&buffer[1]);
					const auto frameLen = IpcFrameHeaderSize + payloadLen;
					if (buffer.size() < frameLen)
					{
						// We do not have enough data to parse whole frame
						return IpcFrame_ptr();
					}

					const auto& data = buffer.substr(0, frameLen);

					buffer.erase(0, frameLen);
				}

				IpcFrameLength Length() const
				{
					return static_cast<IpcFrameLength>(m_data.length());
				}

				const std::string& Payload() const
				{
					return m_payload;
				}

				std::string Payload()
				{
					return m_payload;
				}

				const std::string& Data() const
				{
					return m_data;
				}

				std::string Data()
				{
					return m_data;
				}

			private:
				IpcFrame(const std::string& data)
					: m_data(data)
				{

				}

			private:
				std::string m_data;
				std::string m_payload;
			};

			typedef std::shared_ptr<IpcFrame> IpcFrame_ptr;
		}
	}
}

#endif // __SYSTEM_NET_IPC_FRAME_HPP__
