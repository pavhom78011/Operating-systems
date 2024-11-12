#include "pch.h"
#include "CppUnitTest.h"
#include "Employee.h"
#include <fstream>
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(Structure)
		{
			employee a;
			a.num = 1;
			strcpy_s(a.name, "John Doe");
			a.hours = 40;
			Assert::AreEqual(1, a.num);
			Assert::AreEqual("John Doe", a.name);
			Assert::AreEqual(40, static_cast<int>(a.hours));
		}
		TEST_METHOD(TextStream)
		{
			ofstream ofst("test.bin", ios::binary);
			Assert::IsTrue(ofst.is_open());
			ofst.close();
			ifstream ifst("test.bin", ios::binary);
			Assert::IsTrue(ifst.is_open());
			ifst.close();
		}
	};
}
