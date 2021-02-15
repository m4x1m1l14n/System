#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace System;

namespace UnitTest
{
	namespace SystemTest
	{
		TEST_CLASS(StringTest)
		{
		public:
			TEST_METHOD(UpperCaseTest)
			{
				const std::map<std::wstring, std::wstring> m =
				{
					{ L"", L"" },
					{ L"Hello world", L"HELLO WORLD" },
					{ L"šťávička nádherná žeľeňučká", L"ŠŤÁVIČKA NÁDHERNÁ ŽEĽEŇUČKÁ" },
					{ L"ŠŤÁvičkaJHIDSL` nádhejie2342];;]\\1á žeľeňu//;,;i78á", L"ŠŤÁVIČKAJHIDSL` NÁDHEJIE2342];;]\\1Á ŽEĽEŇU//;,;I78Á" }
				};

				for (const auto& x : m)
				{
					auto actual = x.first;
					auto expected = x.second;

					Assert::AreEqual(expected, String::UpperCase(actual));
				}
			}

			TEST_METHOD(LowerCaseTest)
			{
				const std::map<std::wstring, std::wstring> m =
				{
					{ L"", L"" },
					{ L"Hello world", L"hello world" },
					{ L"šťávička nádherná žeľeňučká", L"šťávička nádherná žeľeňučká" },
					{ L"ŠŤÁvičkaJHIDSL` nádhejie2342];;]\\1á žeľeňu//;,;i78á", L"šťávičkajhidsl` nádhejie2342];;]\\1á žeľeňu//;,;i78á" }
				};

				for (const auto& x : m)
				{
					auto actual = x.first;
					auto expected = x.second;

					Assert::AreEqual(expected, String::LowerCase(actual));
				}
			}
		};
	}
}
