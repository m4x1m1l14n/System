#include <System\TimeoutException.hpp>

namespace System
{
	TimeoutException::TimeoutException()
	{

	}

	TimeoutException::~TimeoutException() throw()
	{
	}

	const char * TimeoutException::what() const throw()
	{
		return "The operation has timed-out.";
	}
}
