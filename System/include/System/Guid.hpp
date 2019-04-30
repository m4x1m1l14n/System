#pragma once

#pragma comment(lib, "Ole32.lib")

#include <objbase.h>

#include <memory>
#include <string>
#include <array>

namespace m4x1m1l14n
{
	namespace System
	{
		class Guid
		{
		public:
			Guid();
			Guid(const GUID& guid);
			Guid(const std::array<unsigned char, 16>& arr);
			Guid(const std::wstring& s);
			Guid(const uint32_t a, const uint16_t b, const uint16_t c, const uint64_t d);

			virtual ~Guid();

			bool operator ==(const Guid& rhs) const;
			bool operator !=(const Guid& rhs) const;

			static Guid NewGuid();

			bool IsEmpty() const;
			std::wstring ToString(const std::wstring& format = std::wstring()) const;
			std::array<unsigned char, 16> ToByteArray() const;

		protected:
			GUID m_guid;
		};
	}
}
