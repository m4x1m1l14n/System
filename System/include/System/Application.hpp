#pragma once

#include <string>
#include <filesystem>

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
