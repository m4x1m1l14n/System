#include <System/Net/Url.hpp>

#include <map>
#include <stdexcept>

/// <summary>
///		Custom case insensitive <c>std::less</c> comparator for map with wstring key
/// </summary>
struct comp
{
	bool operator() (const std::string& lhs, const std::string& rhs) const
	{
		return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

namespace System
{
	namespace Net
	{
		namespace details
		{

		}

		// NOTE
		//	Do not chain default Ctor
		Url::Url()
		{

		}

		Url::Url(const std::string & url)
		{
			if (url.empty())
			{
				throw std::invalid_argument("url cannot be empty!");
			}

			/*
			// TODO Maybe in future
			// Find & parse Url scheme
			auto iter = url.begin();

			do
			{
				if (*iter == ':')
				{
					auto next = iter + 1;
					if (*next == '/')
					{
						m_scheme = std::string(url.begin(), iter - 1);
						// TODO Validate scheme
						m_port = this->GetDefaultPort(m_scheme);
					}

					break;
				}

				++iter;

			} while (iter != url.end());
			*/
		}

		Url::Url(const std::string & scheme, const std::string & host)
			: Url(scheme, host, GetDefaultPort(scheme))
		{
		}

		Url::Url(int port)
			: Url(std::string(), port)
		{

		}

		Url::Url(const std::string & host, int port)
			: Url(std::string(), host, port)
		{
		}

		Url::Url(const std::string & scheme, const std::string & host, int port)
			: Url(scheme, host, port, std::string())
		{

		}

		Url::Url(const std::string & scheme, const std::string & host, const std::string & path)
			: Url(scheme, host, this->GetDefaultPort(scheme), path)
		{
		}

		Url::Url(const std::string & scheme, const std::string & host, int port, const std::string & path)
			: m_scheme(scheme)
			, m_host(host)
			, m_port(port)
			, m_path(path)
		{

		}

		/*Url::~Url()
		{

		}*/

		Url Url::Parse(const std::string & s)
		{
			return Url();
		}

		std::string Url::getHost() const
		{
			return m_host;
		}

		int Url::getPort() const
		{
			return m_port;
		}

		bool Url::IsEmpty() const
		{
			return m_host.empty();
		}

		int Url::GetDefaultPort(const std::string & scheme)
		{
			static const std::map<std::string, int, comp> ports =
			{
				{ "http", 80 },
				{ "https", 443 }
			};

			auto iter = ports.find(scheme);
			if (iter == ports.end())
			{
				throw std::invalid_argument("Unknown scheme \"" + scheme + "\"");
			}

			return iter->second;
		}

		std::string Url::getScheme() const
		{
			return m_scheme;
		}

		std::string Url::Path() const
		{
			return m_path;
		}
	}
}
