#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace ::System::Drawing;

namespace UnitTest
{
	namespace System
	{
		namespace Drawing
		{
			TEST_CLASS(SizeTest)
			{
			public:
				TEST_METHOD(DefaultCtor)
				{
					Size sz;

					Assert::IsTrue(sz.IsEmpty());
					Assert::AreEqual(sz.Height, 0);
					Assert::AreEqual(sz.Width, 0);

					sz = Size(100, 100);

					Assert::IsFalse(sz.IsEmpty());
					Assert::AreEqual(sz.Width, 100);
					Assert::AreEqual(sz.Height, 100);

					sz += Size(50, 50);

					Assert::AreEqual(sz.Width, 150);
					Assert::AreEqual(sz.Height, 150);

					sz -= Size(50, 50);

					Assert::AreEqual(sz.Width, 100);
					Assert::AreEqual(sz.Height, 100);
				}
			};
		}
	}
}
