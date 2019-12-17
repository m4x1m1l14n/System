#pragma once

#include <string>
#include <experimental/filesystem>

namespace std
{
	namespace filesystem = ::std::experimental::filesystem;
}

namespace System
{
	class Application
	{
	public:
		static std::filesystem::path GetFileName();
		static std::filesystem::path GetFilePath();

	private:
		Application()
		{}
	};
}
