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
					const auto& payload = message->Data();
					// NOTE By protocol payload length is 32 bit!
					const auto payloadLen = static_cast<std::uint32_t>(payload.length());

					/*std::string data;

					data.reserve(IpcMessageHeaderSize + message.length());

					data = IpcMessageStart;
					data.append(reinterpret_cast<const char*>(&len), sizeof(len));
					data.append(message);*/

					std::string data;

					data.reserve(sizeof(id) + payload.length());

					data.append(reinterpret_cast<const char*>(&id));
					data.append(payload);

					m_data = std::move(data);
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
				std::string m_data;
			};

			typedef std::shared_ptr<IpcFrame> IpcFrame_ptr;
		}
	}
}

#endif // __SYSTEM_NET_IPC_FRAME_HPP__
