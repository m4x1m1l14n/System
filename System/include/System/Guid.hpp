#pragma once

#pragma comment(lib, "Ole32.lib")

#include <memory>

namespace m4x1m1l14n
{
	namespace System
	{
		class Guid;

		typedef std::shared_ptr<Guid> Guid_ptr;

		class Guid
		{
		public:
			Guid();

			static Guid NewGuid();
		};
	}
}
