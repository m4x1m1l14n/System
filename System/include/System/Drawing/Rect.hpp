#pragma once

#include <type_traits>

namespace System
{
	namespace Drawing
	{
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		class _Rect
		{
		public:
			_Rect()
				: _Rect(T(), T(), T(), T())
			{

			}

			_Rect(const _Point<T>& position, const _Size<T>& size)
				: m_x(position.X)
				, m_y(position.Y)
				, m_width(size.Width)
				, m_height(size.Height)
			{

			}

			_Rect(T x, T y, T width, T height)
				: m_x(x)
				, m_y(y)
				, m_width(width)
				, m_height(height)
			{
				// TODO: normalize square
			}

			bool operator== (const _Rect<T>& rhs) const
			{
				return (
					m_x == rhs.m_x &&
					m_y == rhs.m_y &&
					m_width == rhs.m_width &&
					m_height == rhs.m_height
					);
			}

			bool operator!= (const _Rect<T>& rhs) const
			{
				return !(*this == rhs);
			}

			bool operator> (const _Rect<T>& rhs) const
			{
				return Area > rhs.Area;
			}

			bool operator>= (const _Rect<T>& rhs) const
			{
				return Area >= rhs.Area;
			}

			bool operator< (const _Rect<T>& rhs) const
			{
				return Area < rhs.Area;
			}

			bool operator<= (const _Rect<T>& rhs) const
			{
				return Area <= rhs.Area;
			}

#if _MSC_VER && !__INTEL_COMPILER

#if defined(_WIN32)

			_Rect(const RECT& rc)
				: m_x(Math::Min(rc.left, rc.right))
				, m_y(Math::Min(rc.top, rc.bottom))
				, m_width((rc.left < rc.right) ? rc.right - rc.left : rc.left - rc.right)
				, m_height((rc.top < rc.bottom) ? rc.bottom - rc.top : rc.top - rc.bottom)
			{

			}

#endif

			__declspec(property(get = GetX, put = SetX)) T X;
			__declspec(property(get = GetY, put = SetY)) T Y;
			__declspec(property(get = GetWidth, put = SetWidth)) T Width;
			__declspec(property(get = GetHeight, put = SetHeight)) T Height;

			__declspec(property(get = GetRight)) T Right;
			__declspec(property(get = GetBottom)) T Bottom;

			__declspec(property(get = GetArea)) T Area;
			__declspec(property(get = GetCircumference)) T Circumference;

#endif

		public:
			T GetX() const { return m_x; }
			void SetX(T x) { m_x = x; }

			T GetY() const { return m_y; }
			void SetY(T y) { m_y = y; }

			T GetWidth() const { return m_width; }
			void SetWidth(T width) { m_width = width; }

			T GetHeight() const { return m_height; }
			void SetHeight(T height) { m_height = height; }

			T GetRight() const { return m_x + m_width; }
			T GetBottom() const { return m_y + m_height; }

			T GetArea() const { return m_width * m_height; }

			T GetCircumference() const { return 2 * (m_width + m_height); }

			_Rect<T> Union(const _Rect<T>& rhs)
			{
				_Rect<T> rc;

				rc.X = Math::Min(m_x, rhs.m_x);
				rc.Y = Math::Min(m_y, rhs.m_y);
				rc.Width = Right > rhs.Right ? Right - rc.X : rhs.Right - rc.X;
				rc.Height = Bottom > rhs.Bottom ? Bottom - rc.Y : rhs.Bottom - rc.Y;

				return rc;
			}

			_Rect<T> Intersect(const _Rect<T>& rhs)
			{

			}

		private:
			T m_x;
			T m_y;
			T m_width;
			T m_height;
		};

		typedef _Rect<int> Rect;
		typedef _Rect<float> RectF;
	}
}
