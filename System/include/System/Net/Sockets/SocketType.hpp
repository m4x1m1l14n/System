#pragma once

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			enum class SocketType
			{
				Unknown = -1,
				Stream = 1,
				Dgram = 2,
				Raw = 3,
				Rdm = 4,
				Seqpacket = 5
			};
		}
	}
}
