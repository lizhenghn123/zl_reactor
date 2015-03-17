#include <iostream>
#include <string>
#include "stl/any.h"
using namespace std;

int main()
{
	{
		zl::stl::any an1;
		an1 = 123;
		int num = zl::stl::any_cast<int>(an1);
		cout << num << "\n";
	}
	{
		zl::stl::any an1;
		an1 = (string)"wwwww";
		string str = zl::stl::any_cast<string>(an1);
		cout << str << "\n";
	}
	{
		zl::stl::any an;
		an = 111;
		int num = zl::stl::any_cast<int>(an);
		cout << num << "\n";

		an = (string)"wer";
		string str = zl::stl::any_cast<string>(an);
		cout << str << "\n";
	}
	{
		const zl::stl::any an1 = "1";
		zl::stl::any_cast<int>(an1);
		zl::stl::any_cast<int>(&an1);
		const zl::stl::any *p1 = &an1;
		zl::stl::any_cast<int>(p1);

		zl::stl::any an2 = "1";
		zl::stl::any_cast<int>(an2);
		zl::stl::any_cast<int>(&an2);
		zl::stl::any *p2 = &an2;
		zl::stl::any_cast<int>(p2);
	}
	system("pause");
}