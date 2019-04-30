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

			TEST_METHOD(Ctor1)
			{
				::System::Guid guid;

				guid = ::System::Guid(L"{00000000-0000-0000-0000-000000000000}");
				Assert::IsTrue(guid.IsEmpty());

				guid = ::System::Guid(L"7f5099f63cc14185b8c33a6c7d01c292");
				guid = ::System::Guid(L"7f5099f6-3cc1-4185-b8c3-3a6c7d01c292");
				guid = ::System::Guid(L"{7f5099f6-3cc1-4185-b8c3-3a6c7d01c292}");
				guid = ::System::Guid(L"  {7f5099f6-3cc1-4185-b8c3-3a6c7d01c292}	");
				guid = ::System::Guid(L"7f5099f6-3cc1-4185-b8c3-3a6c7d01c292}");
				guid = ::System::Guid(L"{7f5099f6-3cc1-4185-b8c3-3a6c7d01c292");
				guid = ::System::Guid(L"(7f5099f6-3cc1-4185-b8c3-3a6c7d01c292)");
				guid = ::System::Guid(L"7f5099f6-3cc1-4185-b8c3-3a6c7d01c292)");
				guid = ::System::Guid(L"(7f5099f6-3cc1-4185-b8c3-3a6c7d01c292");
				guid = ::System::Guid(L"{0x7f5099f6,0x3cc1,0x4185,{0xb8,0xc3,0x3a,0x6c,0x7d,0x01,0xc2,0x92}}");
				guid = ::System::Guid(L"0x7f5099f6,0x3cc1,0x4185,{0xb8,0xc3,0x3a,0x6c,0x7d,0x01,0xc2,0x92}}");
				guid = ::System::Guid(L"0x7f5099f6,0x3cc1,0x4185,{0xb8,0xc3,0x3a,0x6c,0x7d,0x01,0xc2,0x92}");

				Assert::IsFalse(guid.IsEmpty());

				Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L""); });
				Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"Yeaaahaa!!!"); });
				Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"{7f5099f6-3cc1-4185-b8c3-b8c3-3a6c7d01c292}"); });
				// Notice "g" in provided GUID
				Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"{7f5099f6-3cc1-4185-b8g3-3a6c7d01c292}"); });
			}

			TEST_METHOD(Ctor2)
			{
				GUID g;

				Assert::AreEqual(S_OK, ::CoCreateGuid(&g));

				::System::Guid guid(g);

				wchar_t buffer[100];

				::StringFromGUID2(g, buffer, ARRAYSIZE(buffer));

				Assert::IsFalse(guid.IsEmpty());
				Assert::AreEqual(std::wstring(buffer), guid.ToString(L"B"));
			}

			TEST_METHOD(NewGuid)
			{
				auto guid = ::System::Guid::NewGuid();

				Assert::IsFalse(guid.IsEmpty());
			}
		};
	}
}