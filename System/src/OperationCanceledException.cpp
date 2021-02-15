#include <System/OperationCanceledException.hpp>

namespace System
{
	OperationCanceledException::OperationCanceledException()
	{

	}

	OperationCanceledException::~OperationCanceledException()
	{

	}

	const char * OperationCanceledException::what() const throw()
	{
		return "Operation cancelled!";
	}
}
