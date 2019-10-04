#pragma once

#include <exception>

namespace System
{
	class TimeoutException
		: public std::exception
	{
	public:
		/// <summary>Default constructor</summary>
		explicit TimeoutException();

		/// <summary>Destructor.</summary>
		/// <remarks>Virtual destructor to allow subclassing.</remarks>
		virtual ~TimeoutException() throw ();

		/// <summary>Returns a pointer to the (constant) error description.</summary>
		/// <returns>A pointer to a const char*.</returns>
		/// <remarks>
		/// The underlying memory of returned pointer is in posession of <c>TimeoutException</c> object.
		/// Callers must not attempt to free the memory.
		/// </remarks>
		virtual const char* what() const throw ();
	};
}
