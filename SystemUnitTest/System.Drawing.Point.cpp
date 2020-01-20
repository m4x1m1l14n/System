#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace ::System::Drawing;

namespace UnitTest
{
	namespace SystemTest
	{
		namespace Drawing
		{
			TEST_CLASS(PointTest)
			{
			public:
				TEST_METHOD(DefaultCtor)
				{
					Point pt;

					Assert::IsTrue(pt.IsEmpty());
					Assert::AreEqual(pt.X, 0);
					Assert::AreEqual(pt.Y, 0);

					pt = Point(12, 12);

					Assert::IsFalse(pt.IsEmpty());
					Assert::AreEqual(pt.X, 12);
					Assert::AreEqual(pt.Y, 12);

					Assert::AreEqual(Point(3, 2).Distance(Point(7, 8)), 7);
					Assert::AreEqual(Point(7, 8).Distance(Point(3, 2)), 7);

					Assert::AreEqual(Point(3, 3).Distance(Point(3, 3)), 0);
					Assert::AreEqual(Point(0, 0).Distance(Point(0, 0)), 0);
					Assert::AreEqual(Point(-100, -120).Distance(Point(-100, -120)), 0);
					Assert::AreEqual(Point(100, 100).Distance(Point(-100, -100)), 282);
				}
			};
		}
	}
}
