#include <System/NotImplementedException.hpp>

namespace System
{
	NotImplementedException::NotImplementedException()
		: NotImplementedException(std::string(), -1)
	{

	}

	NotImplementedException::NotImplementedException(const std::string & function, int line)
	{
		m_message = function.empty()
			? "Call on not implemented function"
			: function + ((line == -1) ? "" : std::to_string(line)) + " is not implemented";
	}

	NotImplementedException::~NotImplementedException()
	{

	}

	const char * NotImplementedException::what() const throw()
	{
		return m_message.c_str();
	}
}
