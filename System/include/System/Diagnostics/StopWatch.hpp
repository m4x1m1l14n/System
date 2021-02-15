#pragma once

#include <memory>
#include <chrono>

namespace System
{
	namespace Diagnostics
	{
		class StopWatch
		{
		public:
			StopWatch()
				: m_begin(std::chrono::high_resolution_clock::now())
			{

			}

			~StopWatch()
			{

			}

			void Reset()
			{
				m_begin = std::chrono::high_resolution_clock::now();
			}

			template <typename _Resolution = std::chrono::milliseconds>
			uint64_t getElapsed() const
			{
				return static_cast<uint64_t>
				(
					std::chrono::duration_cast<_Resolution>(std::chrono::high_resolution_clock::now() - m_begin).count()
				);
			}

			static StopWatch StartNew()
			{
				return StopWatch();
			}

		private:
			std::chrono::high_resolution_clock::time_point m_begin;
		};
	}
}
