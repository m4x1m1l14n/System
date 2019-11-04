#pragma once

#include <string>

namespace System
{
	class TimeSpan
	{
	public:
		static const long long TicksPerDay = 864000000000;
		static const long long TicksPerHour = 36000000000;
		static const long long TicksPerMinute = 600000000;
		static const long long TicksPerSecond = 10000000;
		static const long long TicksPerMillisecond = 10000;

	public:
		TimeSpan();
		TimeSpan(long long ticks);
		TimeSpan(long long hours, long long minutes, long long seconds);
		TimeSpan(long long days, long long hours, long long minutes, long long seconds);
		TimeSpan(long long days, long long hours, long long minutes, long long seconds, long long milliseconds);
		
		virtual ~TimeSpan();

		// Properties
		static TimeSpan MaxValue();
		static TimeSpan MinValue();

		static TimeSpan Zero();

		double GetTotalDays() const;
		double GetTotalHours() const;
		double GetTotalMinutes() const;
		double GetTotalSeconds() const;
		double GetTotalMilliseconds() const;

		long long GetTicks() const;
		long long GetDays() const;
		long long GetHours() const;
		long long GetMinutes() const;
		long long GetSeconds() const;
		long long GetMilliseconds() const;

#if defined(_WIN32) && 0
		__declspec(property(get = GetDays)) long long Days;
		__declspec(property(get = GetTotalDays)) double TotalDays;
		__declspec(property(get = GetHours)) long long Hours;
		__declspec(property(get = GetTotalHours)) double TotalHours;
		__declspec(property(get = GetMinutes)) long long Minutes;
		__declspec(property(get = GetTotalMinutes)) double TotalMinutes;
		__declspec(property(get = GetSeconds)) long long Seconds;
		__declspec(property(get = GetTotalSeconds)) double TotalSeconds;
		__declspec(property(get = GetMilliseconds)) long long Milliseconds;
		__declspec(property(get = GetTotalMilliseconds)) double TotalMilliseconds;
		__declspec(property(get = GetTicks)) long long Ticks;
#endif

		TimeSpan Add(const TimeSpan& span) const;
		TimeSpan Subtract(const TimeSpan& span) const;
		TimeSpan Negate() const;
		TimeSpan Multiply(double factor) const;
		double Divide(const TimeSpan& span) const;
		TimeSpan Divide(double divisor) const;
		TimeSpan Duration() const;

		static TimeSpan FromDays(double value);
		static TimeSpan FromHours(double value);
		static TimeSpan FromMinutes(double value);
		static TimeSpan FromSeconds(double value);
		static TimeSpan FromMilliseconds(double value);
		static TimeSpan FromTicks(long long ticks);

		TimeSpan& operator+= (const TimeSpan& rhs);
		TimeSpan operator+ (const TimeSpan& rhs) const;

		TimeSpan& operator-= (const TimeSpan& rhs);
		TimeSpan operator- (const TimeSpan& rhs) const;
		
		double operator/ (const TimeSpan& rhs) const;
		TimeSpan operator/ (double divisor) const;
		TimeSpan& operator/= (double divisor);

		bool operator== (const TimeSpan& rhs) const;
		
		bool operator> (const TimeSpan& rhs) const;
		bool operator>= (const TimeSpan& rhs) const;

		bool operator< (const TimeSpan& rhs) const;
		bool operator<= (const TimeSpan& rhs) const;

		std::string ToString(const std::string& format = "c");

	private:
		long long m_ticks;
	};
}

