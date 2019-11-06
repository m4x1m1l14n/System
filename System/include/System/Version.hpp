#pragma once

#include <string>

namespace System
{
	class Version
	{
	public:
		Version();
		Version(const std::string& ver);
		Version(unsigned int major);
		Version(unsigned int major, unsigned int minor);
		Version(unsigned int major, unsigned int minor, unsigned int build);
		Version(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision);
		virtual ~Version();

		bool operator>(const Version& obj) const;
		bool operator<(const Version& obj) const;
		bool operator>=(const Version& obj) const;
		bool operator<=(const Version& obj) const;
		bool operator==(const Version& obj) const;
		bool operator!=(const Version& obj) const;
		operator bool() const;

		bool IsEmpty() const;

		unsigned int getMajor() const;
		unsigned int getMinor() const;
		unsigned int getBuild() const;
		unsigned int getRevision() const;

		std::string ToString(unsigned int n = 3) const;

	private:
		static int CompareVersions(const System::Version& a, const System::Version& b);

	private:
		unsigned int _ver[4];
	};
}
