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
