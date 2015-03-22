#include <iostream>
#include <string>
#include "stl/array.h"
using namespace zl::stl;

int main()
{
    zl::stl::array<int, 10> a, b;
    b.fill(1);

    b.back() += 1;
    b.front() -= 20;

    for (size_t i = 0; i < b.size(); ++i)
    {
        std::cout << ((i % 2 == 0) ? b[i] : b.at(i)) << ", ";
    }
    std::cout << "\n";

    a == b;
    a != b;
    a < b;
    a <= b;
    a > b;
    a >= b;

    //zl::stl::array<int, 0> az;   // 'az' uses undefined class 'zl::stl::array<int,0>'

    system("pause");
    return 0;
}
