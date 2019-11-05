#pragma once

#pragma comment(lib, "Ole32.lib")

#include <memory>
#include <string>
#include <array>

namespace System
{
	class Guid
	{
	public:
		Guid();
		Guid(const std::array<unsigned char, 16>& arr);
		Guid(const std::string& s);
		Guid(const uint32_t a, const uint16_t b, const uint16_t c, const uint64_t d);

		virtual ~Guid();

		bool operator ==(const Guid& rhs) const;
		bool operator !=(const Guid& rhs) const;
		operator bool() const;

		static Guid NewGuid();

		bool IsEmpty() const;
		std::string ToString(const std::string& format = std::string()) const;
		std::array<unsigned char, 16> ToArray() const;

	protected:
		std::array<unsigned char, 16> m_data;
	};
}
