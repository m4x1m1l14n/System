#pragma once

#include <string>
#include <exception>

namespace System
{
	class OperationCanceledException
		: public std::exception
	{
	public:
		OperationCanceledException();

		virtual ~OperationCanceledException();

		virtual const char* what() const throw();
	};
}
