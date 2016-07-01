#include <iostream>
#include <assert.h>
#include "zlreactor/net/http/UriQueryParams.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void test1()
{
    UriQueryParams params;
    assert(params.parse("a=1&b=2&c=%FF%FE&d"));
    assert(4 == params.count());
    assert("a" == params.get(0).name());
    assert("1" == params.get(0).value());

    assert("b" == params.get(1).name());
    assert("2" == params.get(1).value());

    assert("c" == params.get(2).name());
    assert("%FF%FE" == params.get(2).value());

    assert("d" == params.get(3).name());
    assert("" == params.get(3).value());

    assert("123" == params.getOrDefaultValue("key", "123"));
}

void test2()
{
    UriQueryParams params;
    params.add(UriQueryParam("len", "124"));

    UriQueryParam param;
    param.name("idx");
    param.value("10");
    params.add(param);

    params.add("islast", "false");
    params.add("key", "123456");
    params.set("key", "555555");
    cout << params.toString() << "\n";
}
int main()
{
    test1();
    test2();
    return 0;
}
