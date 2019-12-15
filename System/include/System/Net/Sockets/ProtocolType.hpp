#ifndef __PROTOCOL_TYPE_HPP__
#define __PROTOCOL_TYPE_HPP__

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			enum class ProtocolType : int
			{
				Unknown = -1,
				Unspecified = 0,
				IP = 0,
				IPv4 = 4,
				Tcp = 6,
				Udp = 17,
				IPv6 = 41
			};
		}
	}
}

#endif // __PROTOCOL_TYPE_HPP__
