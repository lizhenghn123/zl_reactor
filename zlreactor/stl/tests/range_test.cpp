#include <iostream>
#include "range.hpp"
using namespace std;
using namespace zl;
using namespace zl::stl;
/***
c++11的range-based for循环可以支持自定义类型的遍历，但是要求自定义类型满足三个条件：
	1. 实现begin()和end()，他们分别用来返回第一个和最后一个元素的迭代器；
	2. 提供迭代终止的方法；
	3.提供遍历range的方法；
***/

void test_range()
{
	cout << "Range(15):";
	for (auto i : Range(15)){
		cout << " " << i;  // 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
	}
	cout << endl;

	cout << "Range(2,6):";
	for (auto i : Range(2, 6)){
		cout << " " << i;  // 2 3 4 5
	}
	cout << endl;

	cout << "Range(10.5, 15.5):";
	for (float i : Range(10.5, 15.5)){
		cout << " " << i;  // 10.5 11.5 12.5 13.5 14.5
	}
	cout << endl;

	cout << "Range(35,27,-1):";
	for (int i : Range(35, 27, -1)){
		cout << " " << i;  // 35 34 33 32 31 30 29 28
	}
	cout << endl;

	cout << "Range(2,8,0.5):";
	for (float i : Range(2, 8, 0.5)){
		cout << " " << i;  // 2 2.5 3 3.5 4 4.5 5 5.5 6 6.5 7 7.5
	}
	cout << endl;

	cout << "Range(8,7,-0.1):";
	for (auto i : Range(8, 7, -0.1)){
		cout << " " << i;  // 8 7.9 7.8 7.7 7.6 7.5 7.4 7.3 7.2 7.1
	}
	cout << endl;

	cout << "Range('a', 'z'):";
	for (auto i : Range('a', 'z'))
	{
		cout << " " << i;  // a b c d e f g h i j k l m n o p q r s t u v w x y
	}
	cout << endl;
}

int main()
{
	test_range();

	system("pause");
	return 0;
}