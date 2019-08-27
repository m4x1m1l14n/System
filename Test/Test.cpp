#pragma comment(lib, "System.lib")

#include <iostream>

#include <System\Guid.hpp>

int main()
{
	auto guid = System::Guid::NewGuid();

	auto s = guid.ToString(L"X");
	auto arr = guid.ToByteArray();

	System::Guid guid2(arr);

	return 0;
}
