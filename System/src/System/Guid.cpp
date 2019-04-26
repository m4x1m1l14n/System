#include <System\Guid.hpp>

#include <objbase.h>

namespace m4x1m1l14n { namespace System
{
	class GuidImpl : public Guid
	{

	};

	Guid::Guid()
	{

	}

	Guid Guid::NewGuid()
	{
		return Guid();
	}
}}