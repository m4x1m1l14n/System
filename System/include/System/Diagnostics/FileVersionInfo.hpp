#pragma once

#pragma comment(lib, "Version.lib")

#include <System\Version.hpp>

namespace System
{
	namespace Diagnostics
	{
		class FileVersionInfo
		{
		protected:
			FileVersionInfo(unsigned char* pFileVersionInfo);

		public:
			~FileVersionInfo();

			System::Version GetFileVersion() const;

			static FileVersionInfo GetVersionInfo(const std::wstring& fileName);

		protected:
			unsigned char* m_pFileVersionInfo;
		};
	}
}
