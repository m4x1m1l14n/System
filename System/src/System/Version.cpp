#include <System\Version.hpp>

#include <sstream>

#define _Ver_Major_Idx 0
#define _Ver_Minor_Idx 1
#define _Ver_Build_Idx 2
#define _Ver_Rev_Idx 3

namespace System
{
	Version::Version()
		: Version(0)
	{
	}

	Version::Version(int major)
		: Version(major, 0)
	{

	}

	Version::Version(int major, int minor)
		: Version(major, minor, 0)
	{

	}

	Version::Version(int major, int minor, int build)
		: Version(major, minor, build, 0)
	{

	}

	Version::Version(int major, int minor, int build, int revision)
	{
		_ver[_Ver_Major_Idx] = major;
		_ver[_Ver_Minor_Idx] = minor;
		_ver[_Ver_Build_Idx] = build;
		_ver[_Ver_Rev_Idx] = revision;
	}

	Version::Version(const std::wstring& ver)
	{
		int r = swscanf_s(
			ver.c_str(),
			L"%d.%d.%d.%d",
			&_ver[_Ver_Major_Idx],
			&_ver[_Ver_Minor_Idx],
			&_ver[_Ver_Build_Idx],
			&_ver[_Ver_Rev_Idx]
		);

		if (r < 0) { r = 0; }

		if (r < 4)
		{
			for (int i = 3; i >= r; --i)
			{
				_ver[i] = 0;
			}
		}
	}

	Version::Version(const std::string& ver)
	{
		int r = sscanf_s(
			ver.c_str(),
			"%d.%d.%d.%d",
			&_ver[_Ver_Major_Idx],
			&_ver[_Ver_Minor_Idx],
			&_ver[_Ver_Build_Idx],
			&_ver[_Ver_Rev_Idx]
		);

		if (r < 0) { r = 0; }

		if (r < 4)
		{
			for (int i = 3; i >= r; --i)
			{
				_ver[i] = 0;
			}
		}
	}

	Version::~Version()
	{

	}

	bool Version::IsEmpty() const
	{
		return (
			(_ver[_Ver_Major_Idx] == 0) &&
			(_ver[_Ver_Minor_Idx] == 0) &&
			(_ver[_Ver_Build_Idx] == 0) &&
			(_ver[_Ver_Rev_Idx] == 0)
			);
	}

	int Version::getMajor() const
	{
		return _ver[_Ver_Major_Idx];
	}

	int Version::getMinor() const
	{
		return _ver[_Ver_Minor_Idx];
	}

	int Version::getBuild() const
	{
		return _ver[_Ver_Build_Idx];
	}

	int Version::getRevision() const
	{
		return _ver[_Ver_Rev_Idx];
	}

	std::wstring Version::ToString(int n/* = 3*/) const
	{
		if (n > 4) { n = 4; }
		if (n < 1) { n = 1; }

		std::wostringstream ss;

		ss << _ver[0];

		for (int i = 1; i < n; ++i)
		{
			ss << L'.' << _ver[i];
		}

		return ss.str();
	}

	int Version::CompareVersions(const System::Version & a, const System::Version & b)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (a._ver[i] > b._ver[i])
			{
				return 1;
			}
			else if (a._ver[i] < b._ver[i])
			{
				return -1;
			}
		}

		return 0;
	}

	bool Version::operator>(const Version& rhs) const
	{
		return CompareVersions(*this, rhs) > 0;
	}

	bool Version::operator<(const Version& rhs) const
	{
		return CompareVersions(*this, rhs) < 0;
	}

	bool Version::operator==(const Version& rhs) const
	{
		return CompareVersions(*this, rhs) == 0;
	}

	bool Version::operator!=(const Version & rhs) const
	{
		return !(*this == rhs);
	}

	Version::operator bool() const
	{
		return !IsEmpty();
	}

	bool Version::operator<=(const Version& rhs) const
	{
		return CompareVersions(*this, rhs) <= 0;
	}

	bool Version::operator>=(const Version& rhs) const
	{
		return CompareVersions(*this, rhs) >= 0;
	}
}
