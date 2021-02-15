#pragma once

#include <System/TimeSpan.hpp>

namespace System
{
	/**
	 * @todo Implement Value member to return initial value
	 */
	class Timeout
	{
	public:
		static const Timeout Infinite;
		static const Timeout Zero;

	protected:
		Timeout(unsigned long long time);

	public:
		Timeout();

		virtual ~Timeout();

		static Timeout ElapseAfter(unsigned long long milliseconds);
		static Timeout ElapseAfter(const TimeSpan& time);

		static Timeout CreateInfinite();
		static Timeout CreateZero();

		void SetElapsed();
		void SetInfinite();

		bool GetIsElapsed() const;
		bool GetIsInfinite() const;

		void ThrowIfElapsed() const;

	private:
		unsigned long long m_time;
	};
};
