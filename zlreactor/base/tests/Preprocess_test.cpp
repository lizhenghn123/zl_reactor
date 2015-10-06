#include <iostream>
#include <assert.h>
#include "base/Preprocess.h"
using namespace std;

int main()
{
    int i = 123;
    assert(string("ABC") == ZL_PP_STRINGIZE(ABC));
    assert(1234 == ZL_PP_JOIN(1, 234));

    cout << ZL_PP_STRINGIZE(i) << "\n";    // cout i, not 123
    cout << ZL_PP_JOIN(123, 47) << "\n";   // 12347

    return 0;
}
