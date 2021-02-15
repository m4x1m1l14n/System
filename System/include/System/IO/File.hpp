#pragma once

#pragma comment(lib, "Bcrypt.lib")

#include <string>

namespace System
{
	namespace IO
	{
		class File
		{
		private:
			File() = delete;
			~File() = delete;

		public:
			static bool Exists(const std::wstring& file);
			static void Rename(const std::wstring& filePath, const std::wstring& newFileName, bool overwrite = false);
			static void Delete(const std::wstring& filePath);
			static bool TryDelete(const std::wstring& filePath);
			static void DeleteIfExists(const std::wstring& filePath);
			static bool Copy(const std::wstring& sourceFileName, const std::wstring& destFileName, bool overwrite = false);
			static std::string ReadAllText(const std::wstring& fileName);
			static bool WriteAllText(const std::wstring& fileName, const std::string& text);
			static __int64 GetSize(const std::wstring& fileName);
			static bool Move(const std::wstring& srcFileName, const std::wstring& dstFileName);
			static std::string Checksum(const std::wstring& file);

			static void Truncate(const std::wstring& file, const __int64 size);
		};
	}
}
