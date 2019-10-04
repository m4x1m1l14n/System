#pragma once

#include <System\TimeSpan.hpp>

namespace System
{
	class Timeout
	{
	public:
		static const Timeout Infinite;
		static const Timeout Zero;

	protected:
		Timeout();
		Timeout(unsigned long long time);

	public:
		virtual ~Timeout();

		static Timeout ElapseAfter(unsigned long long milliseconds);
		static Timeout ElapseAfter(const TimeSpan& time);

		static Timeout CreateInfinite();
		static Timeout CreateZero();

		void SetElapsed();
		void SetInfinite();

		bool GetIsElapsed() const;
		bool GetIsInfinite() const;

		__declspec(property(get = GetIsElapsed)) bool IsElapsed;
		__declspec(property(get = GetIsInfinite)) bool IsInfinite;

		void ThrowIfElapsed() const;

	private:
		unsigned long long m_time;
	};
};
