#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	namespace SystemTest
	{
		TEST_CLASS(VersionTest)
		{
		public:
			TEST_METHOD(DefaultCtor)
			{
				::System::Version version;

				Assert::IsFalse(version);
				Assert::IsTrue(version.IsEmpty());
				Assert::AreEqual(0u, version.getMajor());
				Assert::AreEqual(0u, version.getMinor());
				Assert::AreEqual(0u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("0.0.0"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("0"), version.ToString(0));
				Assert::AreEqual(std::string("0.0.0.0"), version.ToString(10));

				Assert::AreEqual(std::string("0"), version.ToString(1));
				Assert::AreEqual(std::string("0.0"), version.ToString(2));
				Assert::AreEqual(std::string("0.0.0"), version.ToString(3));
				Assert::AreEqual(std::string("0.0.0.0"), version.ToString(4));
			}

			TEST_METHOD(Ctor1)
			{
				::System::Version version(3);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(3u, version.getMajor());
				Assert::AreEqual(0u, version.getMinor());
				Assert::AreEqual(0u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("3.0.0"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("3"), version.ToString(0));
				Assert::AreEqual(std::string("3.0.0.0"), version.ToString(10));

				Assert::AreEqual(std::string("3"), version.ToString(1));
				Assert::AreEqual(std::string("3.0"), version.ToString(2));
				Assert::AreEqual(std::string("3.0.0"), version.ToString(3));
				Assert::AreEqual(std::string("3.0.0.0"), version.ToString(4));
				
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid(""); });
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid("Yeaaahaa!!!"); });
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid("{7f5099f6-3cc1-4185-b8c3-b8c3-3a6c7d01c292}"); });
				//// Notice "g" in provided GUID
				//Assert::ExpectException<std::invalid_argument>([] { ::System::Guid guid("{7f5099f6-3cc1-4185-b8g3-3a6c7d01c292}"); });
			}

			TEST_METHOD(Ctor2)
			{
				::System::Version version(3, 100);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(3u, version.getMajor());
				Assert::AreEqual(100u, version.getMinor());
				Assert::AreEqual(0u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("3.100.0"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("3"), version.ToString(0));
				Assert::AreEqual(std::string("3.100.0.0"), version.ToString(10));

				Assert::AreEqual(std::string("3"), version.ToString(1));
				Assert::AreEqual(std::string("3.100"), version.ToString(2));
				Assert::AreEqual(std::string("3.100.0"), version.ToString(3));
				Assert::AreEqual(std::string("3.100.0.0"), version.ToString(4));
			}

			TEST_METHOD(Ctor3)
			{
				::System::Version version(3, 100, 14);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(3u, version.getMajor());
				Assert::AreEqual(100u, version.getMinor());
				Assert::AreEqual(14u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("3.100.14"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("3"), version.ToString(0));
				Assert::AreEqual(std::string("3.100.14.0"), version.ToString(10));

				Assert::AreEqual(std::string("3"), version.ToString(1));
				Assert::AreEqual(std::string("3.100"), version.ToString(2));
				Assert::AreEqual(std::string("3.100.14"), version.ToString(3));
				Assert::AreEqual(std::string("3.100.14.0"), version.ToString(4));
			}

			TEST_METHOD(Ctor4)
			{
				::System::Version version(3, 100, 14, 2);

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(3u, version.getMajor());
				Assert::AreEqual(100u, version.getMinor());
				Assert::AreEqual(14u, version.getBuild());
				Assert::AreEqual(2u, version.getRevision());
				Assert::AreEqual(std::string("3.100.14"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("3"), version.ToString(0));
				Assert::AreEqual(std::string("3.100.14.2"), version.ToString(10));

				Assert::AreEqual(std::string("3"), version.ToString(1));
				Assert::AreEqual(std::string("3.100"), version.ToString(2));
				Assert::AreEqual(std::string("3.100.14"), version.ToString(3));
				Assert::AreEqual(std::string("3.100.14.2"), version.ToString(4));
			}

			TEST_METHOD(CtorString)
			{
				::System::Version version("25.16.2577.12");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(25u, version.getMajor());
				Assert::AreEqual(16u, version.getMinor());
				Assert::AreEqual(2577u, version.getBuild());
				Assert::AreEqual(12u, version.getRevision());
				Assert::AreEqual(std::string("25.16.2577"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("25"), version.ToString(0));
				Assert::AreEqual(std::string("25.16.2577.12"), version.ToString(10));

				Assert::AreEqual(std::string("25"), version.ToString(1));
				Assert::AreEqual(std::string("25.16"), version.ToString(2));
				Assert::AreEqual(std::string("25.16.2577"), version.ToString(3));
				Assert::AreEqual(std::string("25.16.2577.12"), version.ToString(4));

				version = ::System::Version("25.16.2577");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(25u, version.getMajor());
				Assert::AreEqual(16u, version.getMinor());
				Assert::AreEqual(2577u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("25.16.2577"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("25"), version.ToString(0));
				Assert::AreEqual(std::string("25.16.2577.0"), version.ToString(10));

				Assert::AreEqual(std::string("25"), version.ToString(1));
				Assert::AreEqual(std::string("25.16"), version.ToString(2));
				Assert::AreEqual(std::string("25.16.2577"), version.ToString(3));
				Assert::AreEqual(std::string("25.16.2577.0"), version.ToString(4));

				version = ::System::Version("25.16");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(25u, version.getMajor());
				Assert::AreEqual(16u, version.getMinor());
				Assert::AreEqual(0u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("25.16.0"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("25"), version.ToString(0));
				Assert::AreEqual(std::string("25.16.0.0"), version.ToString(10));

				Assert::AreEqual(std::string("25"), version.ToString(1));
				Assert::AreEqual(std::string("25.16"), version.ToString(2));
				Assert::AreEqual(std::string("25.16.0"), version.ToString(3));
				Assert::AreEqual(std::string("25.16.0.0"), version.ToString(4));

				version = ::System::Version("25");

				Assert::IsTrue(version);
				Assert::IsFalse(version.IsEmpty());
				Assert::AreEqual(25u, version.getMajor());
				Assert::AreEqual(0u, version.getMinor());
				Assert::AreEqual(0u, version.getBuild());
				Assert::AreEqual(0u, version.getRevision());
				Assert::AreEqual(std::string("25.0.0"), version.ToString());
				// Out of range input
				Assert::AreEqual(std::string("25"), version.ToString(0));
				Assert::AreEqual(std::string("25.0.0.0"), version.ToString(10));

				Assert::AreEqual(std::string("25"), version.ToString(1));
				Assert::AreEqual(std::string("25.0"), version.ToString(2));
				Assert::AreEqual(std::string("25.0.0"), version.ToString(3));
				Assert::AreEqual(std::string("25.0.0.0"), version.ToString(4));
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
					Assert::AreEqual(0u, version.getMajor());
					Assert::AreEqual(0u, version.getMinor());
					Assert::AreEqual(0u, version.getBuild());
					Assert::AreEqual(0u, version.getRevision());
					Assert::AreEqual(std::string("0.0.0"), version.ToString());
					// Out of range input
					Assert::AreEqual(std::string("0"), version.ToString(0));
					Assert::AreEqual(std::string("0.0.0.0"), version.ToString(10));

					Assert::AreEqual(std::string("0"), version.ToString(1));
					Assert::AreEqual(std::string("0.0"), version.ToString(2));
					Assert::AreEqual(std::string("0.0.0"), version.ToString(3));
					Assert::AreEqual(std::string("0.0.0.0"), version.ToString(4));
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