#include <System\TimeSpan.hpp>

#include <sstream>
#include <iomanip>

#define _MILLISECONDS(ms)	(ms * 10000i64)
#define _SECONDS(s)			(s * _MILLISECONDS(1000))
#define _MINUTES(m)			(m * _SECONDS(60))
#define _HOURS(h)			(h * _MINUTES(60))
#define _DAYS(d)			(d * _HOURS(24))

namespace System
{
	TimeSpan::TimeSpan()
		: TimeSpan(0)
	{

	}

	TimeSpan::TimeSpan(long long ticks)
		: m_ticks(ticks)
	{
		
	}

	TimeSpan::TimeSpan(long long hours, long long minutes, long long seconds)
	{
		m_ticks = _HOURS(hours) + _MINUTES(minutes) + _SECONDS(seconds);
	}

	TimeSpan::TimeSpan(long long days, long long hours, long long minutes, long long seconds)
	{
		m_ticks = _DAYS(days) + _HOURS(hours) + _MINUTES(minutes) + _SECONDS(seconds);
	}

	TimeSpan::TimeSpan(long long days, long long hours, long long minutes, long long seconds, long long milliseconds)
	{
		m_ticks = _DAYS(days) + _HOURS(hours) + _MINUTES(minutes) + _SECONDS(seconds) + _MILLISECONDS(milliseconds);
	}

	TimeSpan::~TimeSpan()
	{

	}

	TimeSpan TimeSpan::MaxValue()
	{
		return TimeSpan(
			std::numeric_limits<long long>::max()
		);
	}

	TimeSpan TimeSpan::MinValue()
	{
		return TimeSpan(
			std::numeric_limits<long long>::min()
		);
	}

	TimeSpan TimeSpan::Zero()
	{
		return TimeSpan(0);
	}

	double TimeSpan::GetTotalDays() const
	{
		return static_cast<double>(m_ticks) / _DAYS(1);
	}

	double TimeSpan::GetTotalHours() const
	{
		return static_cast<double>(m_ticks) / _HOURS(1);
	}

	double TimeSpan::GetTotalMinutes() const
	{
		return static_cast<double>(m_ticks) / _MINUTES(1);
	}

	double TimeSpan::GetTotalSeconds() const
	{
		return static_cast<double>(m_ticks) / _SECONDS(1);
	}

	double TimeSpan::GetTotalMilliseconds() const
	{
		return static_cast<double>(m_ticks) / _MILLISECONDS(1);
	}

	long long TimeSpan::GetTicks() const
	{
		return m_ticks;
	}

	long long TimeSpan::GetDays() const
	{
		return static_cast<long long>(GetTotalDays());
	}

	long long TimeSpan::GetHours() const
	{
		return static_cast<long long>((m_ticks - (m_ticks / _DAYS(1)) * _DAYS(1)) / _HOURS(1));
	}

	long long TimeSpan::GetMinutes() const
	{
		return static_cast<long long>((m_ticks - (m_ticks / _HOURS(1)) * _HOURS(1)) / _MINUTES(1));
	}

	long long TimeSpan::GetSeconds() const
	{
		return static_cast<long long>((m_ticks - (m_ticks / _MINUTES(1)) * _MINUTES(1)) / _SECONDS(1));
	}

	long long TimeSpan::GetMilliseconds() const
	{
		return static_cast<long long>((m_ticks - (m_ticks / _SECONDS(1)) * _SECONDS(1)) / _MILLISECONDS(1));
	}

	TimeSpan TimeSpan::Add(const TimeSpan & span) const
	{
		return TimeSpan(m_ticks + span.m_ticks);
	}

	TimeSpan TimeSpan::Subtract(const TimeSpan & span) const
	{
		return TimeSpan(m_ticks - span.m_ticks);
	}

	TimeSpan TimeSpan::Negate() const
	{
		return TimeSpan(m_ticks * -1);
	}

	TimeSpan TimeSpan::Multiply(double factor) const
	{
		return TimeSpan(static_cast<long long>(static_cast<double>(m_ticks) * factor));
	}

	double TimeSpan::Divide(const TimeSpan & span) const
	{
		return static_cast<double>(m_ticks) / span.m_ticks;
	}

	TimeSpan TimeSpan::Divide(double divisor) const
	{
		return TimeSpan(static_cast<long long>(static_cast<double>(m_ticks) / divisor));
	}

	TimeSpan TimeSpan::Duration() const
	{
		return TimeSpan((m_ticks < 0) ? (m_ticks * -1) : m_ticks);
	}

	TimeSpan TimeSpan::FromDays(double value)
	{
		return TimeSpan(static_cast<long long>(value * _DAYS(1)));
	}

	TimeSpan TimeSpan::FromHours(double value)
	{
		return TimeSpan(static_cast<long long>(value * _HOURS(1)));
	}

	TimeSpan TimeSpan::FromMinutes(double value)
	{
		return TimeSpan(static_cast<long long>(value * _MINUTES(1)));
	}

	TimeSpan TimeSpan::FromSeconds(double value)
	{
		return TimeSpan(static_cast<long long>(value * _SECONDS(1)));
	}

	TimeSpan TimeSpan::FromMilliseconds(double value)
	{
		return TimeSpan(static_cast<long long>(value * _MILLISECONDS(1)));
	}

	TimeSpan TimeSpan::FromTicks(long long ticks)
	{
		return TimeSpan(ticks);
	}

	TimeSpan & TimeSpan::operator+=(const TimeSpan & rhs)
	{
		m_ticks += rhs.m_ticks;

		return (*this);
	}

	TimeSpan TimeSpan::operator+(const TimeSpan & rhs) const
	{
		return TimeSpan(m_ticks + rhs.m_ticks);
	}

	TimeSpan & TimeSpan::operator-=(const TimeSpan & rhs)
	{
		m_ticks -= rhs.m_ticks;

		return (*this);
	}

	TimeSpan TimeSpan::operator-(const TimeSpan & rhs) const
	{
		return TimeSpan(m_ticks - rhs.m_ticks);
	}

	double TimeSpan::operator/(const TimeSpan & rhs) const
	{
		return static_cast<double>(m_ticks) / rhs.m_ticks;
	}

	TimeSpan TimeSpan::operator/(double divisor) const
	{
		return TimeSpan(static_cast<long long>(static_cast<double>(m_ticks) / divisor));
	}

	TimeSpan & TimeSpan::operator/=(double divisor)
	{
		m_ticks = static_cast<long long>(static_cast<double>(m_ticks) / divisor);

		return (*this);
	}

	bool TimeSpan::operator==(const TimeSpan & rhs) const
	{
		return m_ticks == rhs.m_ticks;
	}

	bool TimeSpan::operator>(const TimeSpan & rhs) const
	{
		return m_ticks > rhs.m_ticks;
	}

	bool TimeSpan::operator>=(const TimeSpan & rhs) const
	{
		return m_ticks >= rhs.m_ticks;
	}

	bool TimeSpan::operator<(const TimeSpan & rhs) const
	{
		return m_ticks < rhs.m_ticks;
	}

	bool TimeSpan::operator<=(const TimeSpan & rhs) const
	{
		return m_ticks <= rhs.m_ticks;
	}

	std::wstring TimeSpan::ToString(const std::wstring & /*format*/)
	{
		std::wostringstream ss;

		if (m_ticks < 0) { ss << L"-"; }

		auto days = GetDays();
		if (days > 0)
		{
			ss << days << L".";
		}

		ss	<< std::setw(2) << std::setfill(L'0') << Hours << L":"
			<< std::setw(2) << std::setfill(L'0') << Minutes << L":"
			<< std::setw(2) << std::setfill(L'0') << Seconds;

		auto milliseconds = GetMilliseconds();
		if (milliseconds > 0)
		{
			ss << L"." << std::setw(3) << std::setfill(L'0') << milliseconds;
		}

		return ss.str();
	}
}
