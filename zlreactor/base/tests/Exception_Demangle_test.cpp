#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include "base/Demangle.h"
#include "base/Exception.h"
using namespace std;
using namespace zl::base;

int /*__cdecl*/ func(int);//windows平台下在函数名前可加__cdecl、__stdcall、__fastcall，默认__cdecl

float  func(float);

int    func(const std::vector<std::string>& vec);

namespace NP1
{
    int func(int);

    class C
    {
    public:
        int func(int);
    };
};

namespace NP2
{
    int func(int);

    class C
    {
    public:
        int func(int);
    };
};

#define IMPLEMENT_ALL   /**打开该宏，则定义以上函数实现*/

void demangle_name(const char* manglename)
{
    char buf[1024] = {'\0'};

    if(zl::base::demangleName(manglename, buf, 1024))
        std::cout << "demangle sucs: " << buf <<"\n------------\n";
    else
        std::cout << "demangle fail: " << manglename <<"\n------------\n";

}
void test_demangle()
{
    int i = 1;
    float f = 1.0;
    std::vector<std::string> vec;
    NP1::C *pac = new NP1::C;
    NP2::C *pbc = new NP2::C;

#if 1
    func(f);
    func(i);

    NP1::func(i);
    pac->func(i);

    NP2::func(i);
    pbc->func(i);

    func(vec);
#endif

    //Windows,VS 2010重整的名字，在linux下解析不成功
    demangle_name("?func@@YAHABV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Z");
    demangle_name("?func@NP1@@YAHH@Z");
    demangle_name("??0H@@QAE@XZ");
    demangle_name("??0H@@QAE@XZ");   

    //下面是linux下gcc编译器重整后的名字，在Windows下解析不成功
    demangle_name("_ZTCN10LogMessage9LogStreamE0_So");  
    demangle_name("_ZN3FooD1Ev");
    demangle_name("_ZNSoD0Ev");
    demangle_name("_ZTCN10LogMessage9LogStreamE0_So");
    demangle_name("_ZN3FooD1Ev");
}

class Bar
{
 public:
  void test()
  {
    throw Exception("oops");
  }
};

void foo()
{
  Bar b;
  b.test();
}

void test_exception()
{
    try
    {
        //throw Exception("test_exception");
    }
    catch (Exception& e)
    {
    	std::cout << e.what() << "\n" << e.stack_trace() << "\n-------------\n";
    }
   try
  {
    foo();
  }
  catch (const Exception& ex)
  {
    printf("reason: %s\n", ex.what());
    printf("stack trace: %s\n", ex.stack_trace());
  }
}

int main(void)
{
    std::cout << "========== test demangle ==========\n";
    test_demangle();

    std::cout << "========== test exception ==========\n";
    test_exception();

    return 0;
}


#ifdef IMPLEMENT_ALL

int func(int) { return 1; }

float func(float) { return (float)1.11; }

int func(const std::vector<std::string>& vec) { return 0; }

namespace NP1
{
    int func(int i) { return 2 / i; }

    int C::func(int) { return 3; }
};

namespace NP2
{
    int func(int) { return 4; }

    int C::func(int) { return 5; throw; }
};

#endif