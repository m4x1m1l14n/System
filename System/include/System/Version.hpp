#pragma once

#include <string>

namespace System
{
	class Version
	{
	public:
		Version();
		Version(const std::string& ver);
		Version(const std::wstring& ver);
		Version(int major);
		Version(int major, int minor);
		Version(int major, int minor, int build);
		Version(int major, int minor, int build, int revision);
		virtual ~Version();

		bool operator>(const Version& obj) const;
		bool operator<(const Version& obj) const;
		bool operator>=(const Version& obj) const;
		bool operator<=(const Version& obj) const;
		bool operator==(const Version& obj) const;
		bool operator!=(const Version& obj) const;
		operator bool() const;

		bool IsEmpty() const;

		int getMajor() const;
		int getMinor() const;
		int getBuild() const;
		int getRevision() const;

		_declspec(property(get = getMajor)) int Major;
		_declspec(property(get = getMinor)) int Minor;
		_declspec(property(get = getBuild)) int Build;
		_declspec(property(get = getRevision)) int Revision;

		std::wstring ToString(int n = 3) const;

	private:
		static int CompareVersions(const System::Version& a, const System::Version& b);

	private:
		int _ver[4];
	};
}
