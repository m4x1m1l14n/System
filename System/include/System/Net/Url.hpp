#pragma once

#include <string>

namespace System
{
	namespace Net
	{
		class Url 
		{
		public:
			Url();
			Url(const std::string& url);
			Url(const std::string& scheme, const std::string& host);
			Url(int port);
			Url(const std::string& host, int port);
			Url(const std::string& scheme, const std::string& host, int port);
			Url(const std::string& scheme, const std::string& host, const std::string& path);
			Url(const std::string& scheme, const std::string& host, int port, const std::string& path);

			// virtual ~Url();

			static Url Parse(const std::string& s);

			/// Getters
			std::string getHost() const;
			int getPort() const;
			std::string getScheme() const;
			std::string Path() const;

			/// Properties
			__declspec(property(get = getHost)) std::string Host;
			__declspec(property(get = getPort)) int Port;
			__declspec(property(get = getScheme)) std::string Scheme;

			/// Helpers
			bool IsEmpty() const;

			static int GetDefaultPort(const std::string& scheme);

		protected:
			const char* m_pScheme;

			std::string m_scheme;
			std::string m_host;
			int m_port;
			std::string m_path;
		};
	}
}
