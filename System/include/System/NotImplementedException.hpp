#pragma once

#include <string>
#include <exception>

namespace System
{
	class NotImplementedException
		: public std::exception
	{
	public:
		NotImplementedException();
		explicit NotImplementedException(const std::string& function, int line);

		virtual ~NotImplementedException();

		virtual const char* what() const throw();

	protected:
		std::string m_message;
	};
}

#define throwNotImplementedException() throw System::NotImplementedException(__FUNCTION__, __LINE__)
