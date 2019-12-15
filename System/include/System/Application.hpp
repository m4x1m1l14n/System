#pragma once

#include <string>

namespace System
{
	class Application
	{
	public:
		static std::wstring GetFileName();
		static std::wstring GetFilePath();
	};
}
