#include <System/Timeout.hpp>

#include <System/TimeoutException.hpp>

#include <chrono>

namespace System
{
	namespace details
	{
		inline std::uint64_t current_time_ms()
		{
			return static_cast<std::uint64_t>(
				std::chrono::time_point_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now()
				).time_since_epoch().count()
			);
		}
	}

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
		return Timeout(details::current_time_ms() + milliseconds);
	}

	Timeout Timeout::ElapseAfter(const TimeSpan & time)
	{
		const auto& duration = time.Duration();

		return ElapseAfter(static_cast<unsigned long long>(duration.GetTotalMilliseconds()));
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
		return this->GetIsInfinite()
			? false
			: details::current_time_ms() > m_time;
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
