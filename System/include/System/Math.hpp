#pragma once

#include <type_traits>

namespace System
{
	namespace Math
	{
		template<
			class T
		>
		const T& Min(const T& a, const T& b)
		{
			return (a < b) ? a : b;
		}

		template<
			class T
		>
		const T& Max(const T& a, const T& b)
		{
			return (a < b) ? b : a;
		}

		template <class T>
		bool InRange(const T& val, const T& min, const T& max)
		{
			return (val >= min && val <= max);
		}

		template <
			typename T, 
			typename = std::enable_if_t<std::is_integral<T>::value>
		>
		inline void Swap(T& _Left, T& _Right)
		{
			_Left ^= _Right;
			_Right ^= _Left;
			_Left ^= _Right;
		}
	}
}
