#pragma once

#include <type_traits>

namespace System
{
	namespace Drawing
	{
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		class _Size
		{
		public:
			_Size()
				: _Size(T(), T())
			{

			}

			_Size(T width, T height)
				: m_width(width)
				, m_height(height)
			{

			}

			operator bool() const
			{
				return !IsEmpty();
			}

			_Size<T>& operator+=(const _Size<T>& rhs)
			{
				return Add(rhs);
			}

			_Size<T>& operator-=(const _Size<T>& rhs)
			{
				return Subtract(rhs);
			}

			__declspec(property(get = GetWidth, put = SetWidth)) T Width;
			__declspec(property(get = GetHeight, put = SetHeight)) T Height;

			T GetWidth() const { return m_width; }
			void SetWidth(T width) { m_width = width; }

			T GetHeight() const { return m_height; }
			void SetHeight(T height) { m_height = height; }

			bool IsEmpty() const
			{
				return m_width == 0 && m_height == 0;
			}

			_Size<T>& Add(const _Size<T>& sz)
			{
				m_width += sz.m_width;
				m_height += sz.m_height;

				return *this;
			}

			_Size<T>& Subtract(const _Size<T>& sz)
			{
				m_width -= sz.m_width;
				m_height -= sz.m_height;

				return *this;
			}

		private:
			T m_width;
			T m_height;
		};

		typedef _Size<int> Size;
		typedef _Size<float> SizeF;
	}
}
