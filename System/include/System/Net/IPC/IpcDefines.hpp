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
			// TODO Rename to IpcMessageFlags
			typedef unsigned char IpcMessageConfig;

			constexpr IpcMessageId IpcRegisterMessageId = 0;

			constexpr char IpcMessageStart = -86; // 0xAA in hex

			constexpr IpcMessageConfig IpcServerMessageFlag = 0b10000000;
			constexpr IpcMessageConfig IpcClientMessageFlag = 0b00000000;

			constexpr IpcMessageConfig IpcRequestFlag = 0b01000000;
			constexpr IpcMessageConfig IpcResponseFlag = 0b00000000;

			constexpr IpcMessageConfig IpcServerRequestFlags = IpcServerMessageFlag | IpcRequestFlag;
			constexpr IpcMessageConfig IpcClientRequestFlags = IpcClientMessageFlag | IpcRequestFlag;

			constexpr IpcClientId IpcInvalidClientId = 0;
			
			// IPC message header
			//	1 byte 0xAA - frame start identificator
			//	4 bytes		- length of message payload
			//	8 bytes		- message id
			constexpr auto IpcMessageHeaderSize = 
				sizeof(IpcMessageStart) +  
				sizeof(IpcMessageLength) +
				sizeof(IpcMessageConfig) +
				sizeof(IpcMessageId);
		}
	}
}

#endif // __SYSTEM_NET_IPC_DEFINES_HPP__
