#pragma once

#pragma comment(lib, "Winhttp.lib")

#include <ppl.h>
#include <ppltasks.h>

#include <string>

#include <System/Net/Url.hpp>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			concurrency::task<std::string> UploadFileAsync(const System::Net::Url& url, const std::string& path, const std::wstring& filePath);
			concurrency::task<std::string> PostRequestAsync(const System::Net::Url& url, const std::string& path, const std::string& data);
		}
	}
}
