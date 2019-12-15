#pragma once

namespace System
{
	namespace Net
	{
		namespace WebSockets
		{
			enum class WebSocketOpCode : unsigned char
			{
				continuation_frame = 0x00,
				text_frame = 0x01,
				binary_frame = 0x02,
				reserved_0x03 = 0x03,
				reserved_0x04 = 0x04,
				reserved_0x05 = 0x05,
				reserved_0x06 = 0x06,
				reserved_0x07 = 0x07,
				connection_close = 0x08,
				ping = 0x09,
				pong = 0x0a,
				reserved_0x0B = 0x0b,
				reserved_0x0C = 0x0c,
				reserved_0x0D = 0x0d,
				reserved_0x0E = 0x0e,
				reserved_0x0F = 0x0f
			};
		}
	}
}
