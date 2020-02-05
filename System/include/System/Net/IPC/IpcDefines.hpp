#ifndef __SYSTEM_NET_IPC_DEFINES_HPP__
#define __SYSTEM_NET_IPC_DEFINES_HPP__


#include <cstdint>
#include <map>
#include <deque>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			/*
				| 63 .... 32 | 31 ...... 0 |
				| Process ID | Instance ID |
			*/
			typedef std::uint64_t IpcClientId;
			typedef std::uint64_t IpcMessageId;
			typedef std::uint32_t IpcMessageLength;

			const IpcMessageId IpcRegisterMessageId = 0;

			const char IpcMessageStart = -86; // 0xAA in hex

			const std::uint64_t IpcServerRequestFlag = 0x8000000000000000;
			const std::uint64_t IpcClientRequestFlag = 0x0000000000000000;
			constexpr IpcClientId IpcInvalidClientId = 0;
			
			// IPC message header
			//	1 byte 0xAA - frame start identificator
			//	4 bytes		- length of message payload
			//	8 bytes		- message id
			const auto IpcMessageHeaderSize = sizeof(IpcMessageStart) + sizeof(IpcMessageLength) + sizeof(IpcMessageId);
		}
	}
}

#endif // __SYSTEM_NET_IPC_DEFINES_HPP__
