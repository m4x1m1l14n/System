#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	namespace System
	{
		TEST_CLASS(GuidTest)
		{
		public:
			TEST_METHOD(DefaultCtor)
			{
				::System::Guid guid;

				Assert::IsTrue(guid.IsEmpty());
				Assert::AreEqual(std::wstring(L"00000000000000000000000000000000"), guid.ToString(L"N"));
				Assert::AreEqual(std::wstring(L"00000000-0000-0000-0000-000000000000"), guid.ToString());
				Assert::AreEqual(std::wstring(L"00000000-0000-0000-0000-000000000000"), guid.ToString(L"D"));
				Assert::AreEqual(std::wstring(L"{00000000-0000-0000-0000-000000000000}"), guid.ToString(L"B"));
				Assert::AreEqual(std::wstring(L"(00000000-0000-0000-0000-000000000000)"), guid.ToString(L"P"));
				Assert::AreEqual(std::wstring(L"{0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}"), guid.ToString(L"X"));
			}
		};
	}
}