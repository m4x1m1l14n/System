#include <System/Guid.hpp>

#include <map>
#include <regex>
#include <random>
#include <iomanip>
#include <sstream>

namespace System
{
	Guid::Guid()
	{
		m_data.fill(0);
	}

	Guid::Guid(const std::array<unsigned char, 16>& arr)
		: m_data(arr)
	{
		
	}

	Guid::Guid(const std::string & s)
	{
		if (s.empty())
		{
			throw std::invalid_argument("input string cannot be empty");
		}

		const auto& pattern = R"(^\s*[{(]?(?:0x)?([0-9a-fA-F]{8})(?:[-,]?(?:0x)?([0-9a-fA-F]{4}))(?:[-,]?(?:0x)?([0-9a-fA-F]{4}))[-,]?\{?(?:0x)?([0-9a-fA-F]{2})[,]?(?:0x)?([0-9a-fA-F]{2})[,-]?(?:0x)?([0-9a-fA-F]{2})(?:,\s?0x)?([0-9a-fA-F]{2})(?:,\s?0x)?([0-9a-fA-F]{2})(?:,\s?0x)?([0-9a-fA-F]{2})(?:,\s?0x)?([0-9a-fA-F]{2})(?:,\s?0x)?([0-9a-fA-F]{2}),?[)}]{0,2}\s*$)";

		std::smatch matches;
		std::regex re(pattern);

		if (std::regex_match(s, matches, re))
		{
			const auto a = std::stoul(matches[1].str(), nullptr, 16);
			
			m_data[ 0] = reinterpret_cast<const unsigned char*>(&a)[3];
			m_data[ 1] = reinterpret_cast<const unsigned char*>(&a)[2];
			m_data[ 2] = reinterpret_cast<const unsigned char*>(&a)[1];
			m_data[ 3] = reinterpret_cast<const unsigned char*>(&a)[0];

			const auto b = static_cast<unsigned short>(std::stoul(matches[2].str(), nullptr, 16));

			m_data[ 4] = reinterpret_cast<const unsigned char*>(&b)[1];
			m_data[ 5] = reinterpret_cast<const unsigned char*>(&b)[0];

			const auto c = static_cast<unsigned short>(std::stoul(matches[3].str(), nullptr, 16));

			m_data[ 6] = reinterpret_cast<const unsigned char*>(&c)[1];
			m_data[ 7] = reinterpret_cast<const unsigned char*>(&c)[0];

			m_data[ 8] = static_cast<unsigned char>(std::stoul(matches[ 4].str(), nullptr, 16));
			m_data[ 9] = static_cast<unsigned char>(std::stoul(matches[ 5].str(), nullptr, 16));
			m_data[10] = static_cast<unsigned char>(std::stoul(matches[ 6].str(), nullptr, 16));
			m_data[11] = static_cast<unsigned char>(std::stoul(matches[ 7].str(), nullptr, 16));
			m_data[12] = static_cast<unsigned char>(std::stoul(matches[ 8].str(), nullptr, 16));
			m_data[13] = static_cast<unsigned char>(std::stoul(matches[ 9].str(), nullptr, 16));
			m_data[14] = static_cast<unsigned char>(std::stoul(matches[10].str(), nullptr, 16));
			m_data[15] = static_cast<unsigned char>(std::stoul(matches[11].str(), nullptr, 16));
		}
		else
		{
			throw std::invalid_argument("input string in wrong format");
		}
	}

	Guid::Guid(const uint32_t a, const uint16_t b, const uint16_t c, const uint64_t d)
	{
		m_data[ 0] = reinterpret_cast<const unsigned char*>(&a)[3];
		m_data[ 1] = reinterpret_cast<const unsigned char*>(&a)[2];
		m_data[ 2] = reinterpret_cast<const unsigned char*>(&a)[1];
		m_data[ 3] = reinterpret_cast<const unsigned char*>(&a)[0];

		m_data[ 4] = reinterpret_cast<const unsigned char*>(&b)[1];
		m_data[ 5] = reinterpret_cast<const unsigned char*>(&b)[0];

		m_data[ 6] = reinterpret_cast<const unsigned char*>(&c)[1];
		m_data[ 7] = reinterpret_cast<const unsigned char*>(&c)[0];

		m_data[ 8] = reinterpret_cast<const unsigned char*>(&d)[7];
		m_data[ 9] = reinterpret_cast<const unsigned char*>(&d)[6];
		m_data[10] = reinterpret_cast<const unsigned char*>(&d)[5];
		m_data[11] = reinterpret_cast<const unsigned char*>(&d)[4];
		m_data[12] = reinterpret_cast<const unsigned char*>(&d)[3];
		m_data[13] = reinterpret_cast<const unsigned char*>(&d)[2];
		m_data[14] = reinterpret_cast<const unsigned char*>(&d)[1];
		m_data[15] = reinterpret_cast<const unsigned char*>(&d)[0];
	}

	Guid::Guid(const uint32_t a, const uint16_t b, const uint16_t c, const std::array<unsigned char, 8>& d)
	{
		m_data[ 0] = reinterpret_cast<const unsigned char*>(&a)[3];
		m_data[ 1] = reinterpret_cast<const unsigned char*>(&a)[2];
		m_data[ 2] = reinterpret_cast<const unsigned char*>(&a)[1];
		m_data[ 3] = reinterpret_cast<const unsigned char*>(&a)[0];

		m_data[ 4] = reinterpret_cast<const unsigned char*>(&b)[1];
		m_data[ 5] = reinterpret_cast<const unsigned char*>(&b)[0];

		m_data[ 6] = reinterpret_cast<const unsigned char*>(&c)[1];
		m_data[ 7] = reinterpret_cast<const unsigned char*>(&c)[0];

		m_data[ 8] = d[0];
		m_data[ 9] = d[1];
		m_data[10] = d[2];
		m_data[11] = d[3];
		m_data[12] = d[4];
		m_data[13] = d[5];
		m_data[14] = d[6];
		m_data[15] = d[7];
	}

#ifdef _WIN32
	Guid::Guid(const GUID& g)
		: Guid(
			static_cast<uint32_t>(g.Data1),
			static_cast<uint16_t>(g.Data2),
			static_cast<uint16_t>(g.Data3),
			std::array<unsigned char, 8>{
				g.Data4[0],
				g.Data4[1],
				g.Data4[2],
				g.Data4[3],
				g.Data4[4],
				g.Data4[5],
				g.Data4[6],
				g.Data4[7]
			}
		)
	{

	}
#endif // !_WIN32

	Guid::~Guid()
	{

	}

	bool Guid::operator==(const Guid & rhs) const
	{
		return m_data == rhs.m_data;
	}

	bool Guid::operator!=(const Guid & rhs) const
	{
		return !(*this == rhs);
	}

	Guid Guid::NewGuid()
	{
		std::random_device device{};
		std::mt19937 engine{device()};
		std::uniform_int_distribution<int> distribution {};

		Guid guid;

		for (auto& x : guid.m_data)
		{
			x = static_cast<unsigned char>(distribution(engine));
		}

		return guid;
	}

	Guid::operator bool() const
	{
		return !this->IsEmpty();
	}

	bool Guid::IsEmpty() const
	{
		for (const auto& x : m_data)
		{
			if (x != 0)
			{
				return false;
			}
		}

		return true;
	}

	std::string Guid::ToString(const std::string & format) const
	{
		/*
		const std::map<std::string, std::string> formats =
		{
			{ "N", "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" },
			{ "D", "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" },
			{ "", "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" },
			{ "B", "{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}" },
			{ "P", "(%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X)" },
			{ "X", "{0x%02X%02X%02X%02X,0x%02X%02X,0x%02X%02X,{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}}" }
		};
		*/

		std::ostringstream ss;

		if (format.empty() || format == "D")
		{
			ss	<< std::setfill('0') << std::hex
				<< std::setw(2) << static_cast<unsigned>(m_data[ 0])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 1])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 2])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 3]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 4])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 5]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 6])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 7]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 8])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 9]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[10])
				<< std::setw(2) << static_cast<unsigned>(m_data[11])
				<< std::setw(2) << static_cast<unsigned>(m_data[12])
				<< std::setw(2) << static_cast<unsigned>(m_data[13])
				<< std::setw(2) << static_cast<unsigned>(m_data[14])
				<< std::setw(2) << static_cast<unsigned>(m_data[15]);
		}
		else if (format == "N")
		{
			ss	<< std::setfill('0') << std::hex
				<< std::setw(2) << static_cast<unsigned>(m_data[ 0])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 1])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 2])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 3])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 4])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 5])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 6])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 7])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 8])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 9])
				<< std::setw(2) << static_cast<unsigned>(m_data[10])
				<< std::setw(2) << static_cast<unsigned>(m_data[11])
				<< std::setw(2) << static_cast<unsigned>(m_data[12])
				<< std::setw(2) << static_cast<unsigned>(m_data[13])
				<< std::setw(2) << static_cast<unsigned>(m_data[14])
				<< std::setw(2) << static_cast<unsigned>(m_data[15]);
		}
		else if (format == "B")
		{
			ss	<< std::setfill('0') << std::hex
				<< "{"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 0])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 1])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 2])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 3]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 4])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 5]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 6])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 7]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 8])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 9]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[10])
				<< std::setw(2) << static_cast<unsigned>(m_data[11])
				<< std::setw(2) << static_cast<unsigned>(m_data[12])
				<< std::setw(2) << static_cast<unsigned>(m_data[13])
				<< std::setw(2) << static_cast<unsigned>(m_data[14])
				<< std::setw(2) << static_cast<unsigned>(m_data[15])
				<< "}";
		}
		else if (format == "P")
		{
			ss	<< std::setfill('0') << std::hex
				<< "("
				<< std::setw(2) << static_cast<unsigned>(m_data[ 0])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 1])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 2])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 3]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 4])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 5]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 6])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 7]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 8])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 9]) << "-"
				<< std::setw(2) << static_cast<unsigned>(m_data[10])
				<< std::setw(2) << static_cast<unsigned>(m_data[11])
				<< std::setw(2) << static_cast<unsigned>(m_data[12])
				<< std::setw(2) << static_cast<unsigned>(m_data[13])
				<< std::setw(2) << static_cast<unsigned>(m_data[14])
				<< std::setw(2) << static_cast<unsigned>(m_data[15])
				<< ")";
		}
		else if (format == "X")
		{
			ss	<< std::setfill('0') << std::hex
				<< "{0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 0])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 1])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 2])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 3]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 4])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 5]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 6])
				<< std::setw(2) << static_cast<unsigned>(m_data[ 7]) << ",{0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 8]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[ 9]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[10]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[11]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[12]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[13]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[14]) << ",0x"
				<< std::setw(2) << static_cast<unsigned>(m_data[15]) << "}"
				<< "}";
		}
		else
		{
			throw std::invalid_argument("Unsupported format \"" + format + "\"");
		}
		
		return ss.str();
	}

	std::array<unsigned char, 16> Guid::ToArray() const
	{
		return m_data;
	}
}
