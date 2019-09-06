#pragma once

#include <type_traits>

namespace System
{
	namespace Drawing
	{
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		class _Point
		{
		public:
			_Point()
				: _Point(T(), T())
			{

			}

			_Point(T x, T y)
				: m_x(x)
				, m_y(y)
			{

			}

			__declspec(property(get = GetX, put = SetX)) T X;
			__declspec(property(get = GetY, put = SetY)) T Y;

			T GetX() const { return m_x; }
			void SetX(T x) { m_x = x; }

			T GetY() const { return m_y; }
			void SetY(T y) { m_y = y; }

			bool IsEmpty() const { return m_x == 0 && m_y == 0; }

			void Offset(const _Point<T>& p)
			{
				Offset(p.m_x, p.m_y);
			}

			void Offset(T dx, T dy)
			{
				m_x += dx;
				m_y += dy;
			}

			T Distance(const _Point<T>& p) const
			{
				return static_cast<T>(std::sqrt(
					std::pow(std::abs(p.m_x - m_x), 2) + std::pow(std::abs(p.m_y - m_y), 2)
				));
			}

		private:
			T m_x;
			T m_y;
		};

		typedef _Point<int> Point;
		typedef _Point<float> PointF;
	}
}