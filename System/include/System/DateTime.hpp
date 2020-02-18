#ifndef __SYSTEM_DATETIME_HPP__
#define __SYSTEM_DATETIME_HPP__

#include <System/TimeSpan.hpp>

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <System/Math.hpp>

namespace System
{
	enum class DateTimeDays : int
	{
		Monday = 1,
		Tuesday = 2,
		Wednesday = 3,
		Thursday = 4,
		Friday = 5,
		Saturday = 6,
		Sunday = 7
	};

	enum class DateTimeMonth : int
	{
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	};

	class DateTime
	{
	private:
		DateTime(uint64_t ms)
		{
			int years = static_cast<int>(static_cast<double>(ms) / 365.25);

			
		}

	public:
		
		DateTime()
			: m_year(0)
			, m_month(0)
			, m_day(0)
			, m_hours(0)
			, m_minutes(0)
			, m_seconds(0)
			, m_milliseconds(0)
		{

		}

		/*
			Default constructed month is January
		*/
		DateTime(int year)
			: DateTime(year, static_cast<int>(DateTimeMonth::January))
		{

		}

		DateTime(int year, int month)
			: DateTime(year, month, 1)
		{

		}

		DateTime(int year, int month, int day)
			: DateTime(year, month, day, 0)
		{

		}

		DateTime(int year, int month, int day, int hours)
			: DateTime(year, month, day, hours, 0)
		{

		}

		DateTime(int year, int month, int day, int hours, int minutes)
			: DateTime(year, month, day, hours, minutes, 0)
		{

		}

		DateTime(int year, int month, int day, int hours, int minutes, int seconds)
			: DateTime(year, month, day, hours, minutes, seconds, 0)
		{

		}

		DateTime(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds)
			: m_year(static_cast<uint16_t>(year))
			, m_month(static_cast<uint8_t>(month))
			, m_day(static_cast<uint8_t>(day))
			, m_hours(static_cast<uint8_t>(hours))
			, m_minutes(static_cast<uint8_t>(minutes))
			, m_seconds(static_cast<uint8_t>(seconds))
			, m_milliseconds(static_cast<uint16_t>(milliseconds))
		{
			if (!Math::InRange(month, 1, 12)) { throw std::invalid_argument("Month must be between 1 and 12"); }
			if (!Math::InRange(day, 1, DaysInMonth(year, month))) { throw std::invalid_argument("Day in " + std::to_string(month) + "/" + std::to_string(year) + " must be between 1 and " + std::to_string(DaysInMonth(year, month))); }
			if (!Math::InRange(hours, 0, 23)) { throw std::invalid_argument("Hour must be between 0 and 23"); }
			if (!Math::InRange(minutes, 0, 59)) { throw std::invalid_argument("Minute must be between 0 and 59"); }
			if (!Math::InRange(seconds, 0, 59)) { throw std::invalid_argument("Second must be between 0 and 59"); }
			if (!Math::InRange(milliseconds, 0, 999)) { throw std::invalid_argument("Millisecond must be between 0 and 999"); }
		}

		DateTime(const DateTime& rhs)
		{
			this->Copy(rhs);
		}

		DateTime& operator=(const DateTime& rhs)
		{
			this->Copy(rhs);

			return *this;
		}

		static DateTime UtcNow()
		{
			return DateTime(
				static_cast<uint64_t>(
					std::chrono::time_point_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now()
						).time_since_epoch().count()
				)
			);
		}

		static DateTime Now()
		{
			throw std::runtime_error("not implemented");
		}

		static DateTime Today()
		{
			return DateTime::Now().getDate();
		}

		static DateTime FromUnixTimestamp(const uint64_t timestamp)
		{
			return DateTime(timestamp * 1000ull);
		}

		uint64_t ToUnixTimestamp() const
		{
			const int leapYears = CountLeapYears(m_year);
			
			return
				leapYears * Days2Ms(366) + (m_year - EpochBeginYear - leapYears) * Days2Ms(365)
				+ Days2Ms(DaysTillMonth(m_year, m_month))
				+ Days2Ms(m_day > 0 ? m_day - 1 : 0)
				+ Hours2Ms(m_hours > 0 ? m_hours - 1 : 0)
				+ Minutes2Ms(m_minutes > 0 ? m_minutes - 1 : 0)
				+ Seconds2Ms(m_seconds > 0 ? m_seconds - 1 : 0)
				/*+ m_milliseconds*/;
		}

		bool IsEmpty() const
		{
			return
				(m_year == 0) &&
				(m_month == 0) &&
				(m_day == 0) &&
				(m_hours == 0) &&
				(m_minutes == 0) &&
				(m_seconds == 0) &&
				(m_milliseconds == 0);
		}

		DateTime getDate() const
		{
			return DateTime(m_year, m_month, m_day);
		}

		int getYear() const
		{
			return m_year;
		}

		int getDay() const
		{
			return m_day;
		}

		int getMonth() const
		{
			return m_month;
		}

		int getHour() const
		{
			return m_hours;
		}

		int getMinute() const
		{
			return m_minutes;
		}

		int getSecond() const
		{
			return m_seconds;
		}

		int getMilliseconds() const
		{
			return m_milliseconds;
		}

		bool operator<(const DateTime& rhs) const
		{
			return Compare(rhs) < 0;
		}

		bool operator<=(const DateTime& rhs) const
		{
			return Compare(rhs) <= 0;
		}

		bool operator>(const DateTime& rhs) const
		{
			return Compare(rhs) > 0;
		}

		bool operator>=(const DateTime& rhs) const
		{
			return Compare(rhs) >= 0;
		}

		bool operator==(const DateTime& rhs) const
		{
			return Compare(rhs) == 0;
		}

		DateTime operator +=(const TimeSpan& span) const
		{
			return this->Add(span);
		}

		DateTime operator + (const TimeSpan& rhs) const
		{

		}

		DateTime operator -= (const TimeSpan& span) const
		{
			return this->Subtract(span);
		}

		DateTime operator - (const TimeSpan& rhs) const
		{

		}

		TimeSpan operator - (const DateTime& rhs) const
		{
			return TimeSpan::FromMilliseconds(this->ToUnixTimestamp() - rhs.ToUnixTimestamp());
		}

		DateTime Add(const TimeSpan& span) const
		{
			throw std::runtime_error("not implemented");
		}

		DateTime Subtract(const TimeSpan& span) const
		{
			throw std::runtime_error("not implemented");
		}

		TimeSpan Subtract(const DateTime& dt) const
		{
			throw std::runtime_error("not implemented");
		}

		DateTime AddMilliseconds(int milliseconds) const
		{

		}

		DateTime AddYears(int years) const
		{
			return DateTime(m_year + years, m_month, m_day, m_hours, m_minutes, m_seconds, m_milliseconds);
		}

		DateTime AddMonths(int months) const
		{
			return DateTime
			(
				m_year + (m_month + months) / 12,
				(m_month + months) % 12,
				m_day,
				m_hours,
				m_minutes,
				m_seconds,
				m_milliseconds
			);
		}

		DateTime AddMonths(double months) const
		{
			throw std::runtime_error("not implemented");
		}

		DateTime AddDays(int days) const
		{
			auto years = (m_day + days) / 365;
			auto months = (m_day + days - (years * 365)) / 12;
			
			return DateTime
			(
				m_year + years,
				m_month + months,
				m_day + days,
				m_hours,
				m_minutes,
				m_seconds,
				m_milliseconds
			);
		}

		std::string ToString(const std::string& format = std::string()) const
		{
			const std::string defaultFormat = "YYYY/MM/dd HH:mm:ss";

			const auto& frmt = format.empty()
				? defaultFormat
				: format;

			auto ptr = frmt.c_str();

			std::stringstream ss;

			ss << std::setfill('0');
			
			auto c = *ptr;
			std::string cmd;

			do
			{
				auto prev = c;
				c = *ptr++;

				if (prev != c)
				{
					if (cmd == "YYYY"){ ss << std::setw(4) << this->getYear(); }
					else if (cmd == "MM") { ss << std::setw(2) << this->getMonth(); }
					else if (cmd == "dd") { ss << std::setw(2) << this->getDay(); }
					else if (cmd == "HH") { ss << std::setw(2) << this->getHour(); }
					else if (cmd == "mm") { ss << std::setw(2) << this->getMinute(); }
					else if (cmd == "ss") { ss << std::setw(2) << this->getSecond(); }
					else if (cmd == "fff") { ss << std::setw(3) << this->getMilliseconds(); }
					else { ss << cmd; }

					cmd.clear();
				}

				cmd += c;
				
			} while (c != '\0');

			return ss.str();
		}

		// Statics
		static constexpr bool IsLeapYear(int year)
		{
			return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
		}

		static constexpr int DaysInYear(int year)
		{
			return IsLeapYear(year) ? 366 : 365;
		}

		static constexpr int DaysInMonth(int year, int month)
		{
			constexpr int daysPerMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

			return (month == 2 && IsLeapYear(year))
				? 29
				: daysPerMonth[(month - 1) % 12];
		}

		static constexpr int DaysTillMonth(int year, int month)
		{
			constexpr int daysTillMonthLeap[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
			constexpr int daysTillMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

			return IsLeapYear(year)
				? daysTillMonthLeap[(month - 1) % 12]
				: daysTillMonth[(month - 1) % 12];
		}

		static constexpr uint64_t Years2Ms(uint64_t years)
		{
			return Days2Ms(years * static_cast<uint64_t>(365));
		}

		static constexpr uint64_t Days2Ms(uint64_t days)
		{
			return Hours2Ms(days * static_cast<uint64_t>(24));
		}

		static constexpr uint64_t Hours2Ms(uint64_t hours)
		{
			return Minutes2Ms(hours * static_cast<uint64_t>(60));
		}

		static constexpr uint64_t Minutes2Ms(uint64_t minutes)
		{
			return Seconds2Ms(minutes * static_cast<uint64_t>(60));
		}

		static constexpr uint64_t Seconds2Ms(uint64_t seconds)
		{
			return (seconds * static_cast<uint64_t>(1000));
		}

	private:
		static constexpr int EpochBeginYear = 1970;

		static constexpr int CountLeapYears(int year)
		{
			return ((year - 1972) + 1) / 4;
		}

		void Copy(const DateTime& rhs)
		{
			m_year = rhs.m_year;
			m_month = rhs.m_month;
			m_day = rhs.m_day;
			m_hours = rhs.m_hours;
			m_minutes = rhs.m_minutes;
			m_seconds = rhs.m_seconds;
			m_milliseconds = rhs.m_milliseconds;
		}

		int Compare(const DateTime& rhs) const
		{
			if (m_year == rhs.m_year)
			{
				if (m_month == rhs.m_month)
				{
					if (m_day == rhs.m_day)
					{
						if (m_hours == rhs.m_hours)
						{
							if (m_minutes == rhs.m_minutes)
							{
								if (m_seconds == rhs.m_seconds)
								{
									if (m_milliseconds == rhs.m_milliseconds)
									{
										return 0;
									}
									else
									{
										return (m_milliseconds > rhs.m_milliseconds) ? 1 : -1;
									}
								}
								else
								{
									return (m_seconds > rhs.m_seconds) ? 1 : -1;
								}
							}
							else
							{
								return (m_minutes > rhs.m_minutes) ? 1 : -1;
							}
						}
						else
						{
							return (m_hours > rhs.m_hours) ? 1 : -1;
						}
					}
					else
					{
						return (m_day > rhs.m_day) ? 1 : -1;
					}
				}
				else
				{
					return (m_month > rhs.m_month) ? 1 : -1;
				}
			}
			else
			{
				return (m_year > rhs.m_year) ? 1 : -1;
			}
		}

	private:
		uint16_t m_year;
		uint8_t m_month;
		uint8_t m_day;
		uint8_t m_hours;
		uint8_t m_minutes;
		uint8_t m_seconds;
		uint16_t m_milliseconds;
	};
}

#endif // __SYSTEM_DATETIME_HPP__
