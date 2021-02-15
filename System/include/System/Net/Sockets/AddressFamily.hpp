#ifndef __ADDRESS_FAMILY_HPP__
#define __ADDRESS_FAMILY_HPP__

namespace System
{
	namespace Net
	{
		namespace Sockets
		{
			enum class AddressFamily : int
			{
				Unknown = -1,
				InterNetwork = 2,
				InterNetworkV6 = 23
			};
		}
	}
}

#endif // __ADDRESS_FAMILY_HPP__
