#include <System\Timeout.hpp>

#include <System\TimeoutException.hpp>

#include <Windows.h>

namespace System
{
	const Timeout Timeout::Infinite = Timeout::CreateInfinite();
	const Timeout Timeout::Zero = Timeout::CreateZero();

	Timeout::Timeout()
		: Timeout(0)
	{

	}

	Timeout::Timeout(unsigned long long time)
		: m_time(time)
	{

	}

	Timeout::~Timeout()
	{

	}

	Timeout Timeout::ElapseAfter(unsigned long long milliseconds)
	{
		return Timeout(::GetTickCount64() + milliseconds);
	}

	Timeout Timeout::ElapseAfter(const TimeSpan & time)
	{
		const auto& duration = time.Duration();

		return ElapseAfter(static_cast<unsigned long long>(duration.TotalMilliseconds));
	}

	inline Timeout Timeout::CreateInfinite()
	{
		return Timeout(
			std::numeric_limits<unsigned long long>::max()
		);
	}

	Timeout Timeout::CreateZero()
	{
		return Timeout();
	}

	void Timeout::SetElapsed()
	{
		m_time = 0;
	}

	void Timeout::SetInfinite()
	{
		m_time = std::numeric_limits<unsigned long long>::max();
	}

	bool Timeout::GetIsElapsed() const
	{
		return IsInfinite 
			? false
			: ::GetTickCount64() > m_time;
	}

	void Timeout::ThrowIfElapsed() const
	{
		if (GetIsElapsed())
		{
			throw TimeoutException();
		}
	}

	bool Timeout::GetIsInfinite() const
	{
		return (m_time == std::numeric_limits<unsigned long long>::max());
	}
}
