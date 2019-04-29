#include <System\Guid.hpp>

#include <map>

namespace m4x1m1l14n { namespace System
{
	Guid::Guid()
	{
		std::memset(&m_guid, 0, sizeof(m_guid));
	}

	Guid::Guid(const GUID & guid)
		: m_guid(guid)
	{

	}

	Guid::Guid(const std::array<unsigned char, 16>& arr)
	{
		auto ptr = reinterpret_cast<unsigned char*>(&m_guid);

		for (const auto& x : arr)
		{
			*ptr = x;
			++ptr;
		}
	}

	Guid::Guid(const std::wstring & s)
	{
		
	}

	Guid::Guid(const uint32_t a, const uint16_t b, const uint16_t c, const uint64_t d)
	{
		m_guid.Data1 = a;
		m_guid.Data2 = b;
		m_guid.Data3 = c;
		
		*reinterpret_cast<uint64_t*>(m_guid.Data4) = d;
	}

	Guid::~Guid()
	{
	}

	bool Guid::operator==(const Guid & rhs) const
	{
		for (auto i = 0; i < sizeof(rhs.m_guid); ++i)
		{
			if (reinterpret_cast<const unsigned char*>(&m_guid) != reinterpret_cast<const unsigned char*>(&rhs.m_guid))
			{
				return false;
			}
		}

		return true;
	}

	bool Guid::operator!=(const Guid & rhs) const
	{
		return !(*this == rhs);
	}

	Guid Guid::NewGuid()
	{
		GUID guid;

		HRESULT hr = ::CoCreateGuid(&guid);
		if (!SUCCEEDED(hr))
		{
			auto ec = std::error_code(static_cast<int>(hr), std::system_category());

			throw std::system_error(ec, "CoCreateGuid() failed.");
		}

		return Guid(guid);
	}

	bool Guid::IsEmpty() const
	{
		for (auto i = 0; i < sizeof(m_guid); ++i)
		{
			if (reinterpret_cast<const unsigned char*>(&m_guid)[i] != 0)
			{
				return false;
			}
		}

		return true;
	}

	std::wstring Guid::ToString(const std::wstring & format) const
	{
		const std::map<std::wstring, std::wstring> formats =
		{
			{ L"N", L"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" },
			{ L"D", L"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" },
			{ L"", L"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" },
			{ L"B", L"{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}" },
			{ L"P", L"(%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X)" },
			{ L"X", L"{0x%02X%02X%02X%02X,0x%02X%02X,0x%02X%02X,{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}}" }
		};

		const auto iter = formats.find(format);
		if (iter != formats.end())
		{
			wchar_t * buffer = nullptr;
			int len = 0;

			do
			{
				len = _snwprintf(buffer, len, iter->second.c_str(),
					reinterpret_cast<const unsigned char*>(&m_guid.Data1)[0],
					reinterpret_cast<const unsigned char*>(&m_guid.Data1)[1],
					reinterpret_cast<const unsigned char*>(&m_guid.Data1)[2],
					reinterpret_cast<const unsigned char*>(&m_guid.Data1)[3],
					reinterpret_cast<const unsigned char*>(&m_guid.Data2)[0],
					reinterpret_cast<const unsigned char*>(&m_guid.Data2)[1],
					reinterpret_cast<const unsigned char*>(&m_guid.Data3)[0],
					reinterpret_cast<const unsigned char*>(&m_guid.Data3)[1],
					m_guid.Data4[0],
					m_guid.Data4[1],
					m_guid.Data4[2],
					m_guid.Data4[3],
					m_guid.Data4[4],
					m_guid.Data4[5],
					m_guid.Data4[6],
					m_guid.Data4[7]
				);

				if (len && !buffer)
				{
					buffer = new wchar_t[len];
				}
				else
				{
					break;
				}

			} while (1);

			std::wstring ret;

			if (buffer && len)
			{
				ret = std::wstring(buffer, len);

				delete[] buffer;
			}

			return ret;
		}

		throw std::invalid_argument("specified format is not supported");
	}

	std::array<unsigned char, 16> Guid::ToByteArray() const
	{
		return
		{
			reinterpret_cast<const unsigned char*>(&m_guid.Data1)[0],
			reinterpret_cast<const unsigned char*>(&m_guid.Data1)[1],
			reinterpret_cast<const unsigned char*>(&m_guid.Data1)[2],
			reinterpret_cast<const unsigned char*>(&m_guid.Data1)[3],
			reinterpret_cast<const unsigned char*>(&m_guid.Data2)[0],
			reinterpret_cast<const unsigned char*>(&m_guid.Data2)[1],
			reinterpret_cast<const unsigned char*>(&m_guid.Data3)[0],
			reinterpret_cast<const unsigned char*>(&m_guid.Data3)[1],
			m_guid.Data4[0],
			m_guid.Data4[1],
			m_guid.Data4[2],
			m_guid.Data4[3],
			m_guid.Data4[4],
			m_guid.Data4[5],
			m_guid.Data4[6],
			m_guid.Data4[7]
		};
	}
}}