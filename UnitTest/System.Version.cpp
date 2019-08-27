#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	namespace System
	{
		TEST_CLASS(VersionTest)
		{
		public:
			TEST_METHOD(DefaultCtor)
			{
				::System::Version version;

				Assert::IsFalse(version);
				Assert::IsTrue(version.IsEmpty());
				Assert::AreEqual(version.Major, 0u);
				Assert::AreEqual(version.Minor, 0u);
				Assert::AreEqual(version.Build, 0u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"0.0.0"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"0"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"0.0.0.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"0"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"0.0"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"0.0.0"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"0.0.0.0"));
			}

			TEST_METHOD(Ctor1)
			{
				::System::Version version(3);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 3u);
				Assert::AreEqual(version.Minor, 0u);
				Assert::AreEqual(version.Build, 0u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"3.0.0"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"3.0.0.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"3.0"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"3.0.0"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"3.0.0.0"));
				
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L""); });
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"Yeaaahaa!!!"); });
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"{7f5099f6-3cc1-4185-b8c3-b8c3-3a6c7d01c292}"); });
				//// Notice "g" in provided GUID
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(L"{7f5099f6-3cc1-4185-b8g3-3a6c7d01c292}"); });
			}

			TEST_METHOD(Ctor2)
			{
				::System::Version version(3, 100);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 3u);
				Assert::AreEqual(version.Minor, 100u);
				Assert::AreEqual(version.Build, 0u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"3.100.0"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"3.100.0.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"3.100"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"3.100.0"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"3.100.0.0"));
			}

			TEST_METHOD(Ctor3)
			{
				::System::Version version(3, 100, 14);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 3u);
				Assert::AreEqual(version.Minor, 100u);
				Assert::AreEqual(version.Build, 14u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"3.100.14"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"3.100.14.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"3.100"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"3.100.14"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"3.100.14.0"));
			}

			TEST_METHOD(Ctor4)
			{
				::System::Version version(3, 100, 14, 2);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 3u);
				Assert::AreEqual(version.Minor, 100u);
				Assert::AreEqual(version.Build, 14u);
				Assert::AreEqual(version.Revision, 2u);
				Assert::AreEqual(version.ToString(), std::wstring(L"3.100.14"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"3.100.14.2"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"3"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"3.100"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"3.100.14"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"3.100.14.2"));
			}

			TEST_METHOD(CtorString)
			{
				::System::Version version("25.16.2577.12");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 25u);
				Assert::AreEqual(version.Minor, 16u);
				Assert::AreEqual(version.Build, 2577u);
				Assert::AreEqual(version.Revision, 12u);
				Assert::AreEqual(version.ToString(), std::wstring(L"25.16.2577"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"25.16.2577.12"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"25.16"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"25.16.2577"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"25.16.2577.12"));

				version = ::System::Version("25.16.2577");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 25u);
				Assert::AreEqual(version.Minor, 16u);
				Assert::AreEqual(version.Build, 2577u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"25.16.2577"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"25.16.2577.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"25.16"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"25.16.2577"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"25.16.2577.0"));

				version = ::System::Version("25.16");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 25u);
				Assert::AreEqual(version.Minor, 16u);
				Assert::AreEqual(version.Build, 0u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"25.16.0"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"25.16.0.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"25.16"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"25.16.0"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"25.16.0.0"));

				version = ::System::Version("25");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(version.Major, 25u);
				Assert::AreEqual(version.Minor, 0u);
				Assert::AreEqual(version.Build, 0u);
				Assert::AreEqual(version.Revision, 0u);
				Assert::AreEqual(version.ToString(), std::wstring(L"25.0.0"));
				// Out of range input
				Assert::AreEqual(version.ToString(0), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(10), std::wstring(L"25.0.0.0"));

				Assert::AreEqual(version.ToString(1), std::wstring(L"25"));
				Assert::AreEqual(version.ToString(2), std::wstring(L"25.0"));
				Assert::AreEqual(version.ToString(3), std::wstring(L"25.0.0"));
				Assert::AreEqual(version.ToString(4), std::wstring(L"25.0.0.0"));
			}

			TEST_METHOD(InvalidInputCtorString)
			{
				const std::vector<std::string> invalidInputs = {
					"",
					"Jack.Daniels.Old.No.7"
				};

				for (const auto& invalidInput : invalidInputs)
				{
					::System::Version version(invalidInput);

					Assert::IsFalse(version);
					Assert::IsTrue(version.IsEmpty());
					Assert::AreEqual(version.Major, 0u);
					Assert::AreEqual(version.Minor, 0u);
					Assert::AreEqual(version.Build, 0u);
					Assert::AreEqual(version.Revision, 0u);
					Assert::AreEqual(version.ToString(), std::wstring(L"0.0.0"));
					// Out of range input
					Assert::AreEqual(version.ToString(0), std::wstring(L"0"));
					Assert::AreEqual(version.ToString(10), std::wstring(L"0.0.0.0"));

					Assert::AreEqual(version.ToString(1), std::wstring(L"0"));
					Assert::AreEqual(version.ToString(2), std::wstring(L"0.0"));
					Assert::AreEqual(version.ToString(3), std::wstring(L"0.0.0"));
					Assert::AreEqual(version.ToString(4), std::wstring(L"0.0.0.0"));
				}
			}

			TEST_METHOD(Operators)
			{
				::System::Version a(3, 4, 1, 1);
				::System::Version b(3, 4, 1, 2);

				Assert::IsTrue(a < b);
				Assert::IsTrue(a <= b);
				Assert::IsTrue(b > a);
				Assert::IsTrue(b >= a);
				Assert::IsTrue(a != b);
				Assert::IsFalse(a == b);

				::System::Version c(3, 4, 1, 1);

				Assert::IsFalse(a < c);
				Assert::IsTrue(a <= c);
				Assert::IsFalse(c > a);
				Assert::IsTrue(c >= a);
				Assert::IsFalse(a != c);
				Assert::IsTrue(a == c);
			}
		};
	}
}