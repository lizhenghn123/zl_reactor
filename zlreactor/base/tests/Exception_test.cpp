#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include "base/Exception.h"
using namespace std;
using namespace zl::base;

namespace test1
{
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
            throw Exception("test_exception");
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
}


namespace test2
{
    int test_flag = 1;  // 1 means SIGSEGV, 2 means SIGFPE

    namespace detail
    {
        void c_do_nothing(float f, int c)
        {
            printf("This is c_do_nothing : %f, %d\n", f, c);
            if (test_flag == 1)
            {
                char *p = (char*)c;
                *p = 'C';     // 段错误
            }
            else if (test_flag == 2)
            {
                c = int(f) / c; // 浮点数例外
            }
            printf("result = %d\n", c);
        }
    }

    void bTwo(int b, const char *str)
    {
        printf("This is bTwo : %d  %s\n", b, str);
        detail::c_do_nothing(b * 1.2, 0);
    }

    void aTest(int a)
    {
        printf("This is aTest : %d\n", a);
        bTwo(a + 1, "hello world");
    }

    void debugBacktrace(int sig)
    {
        const static int array_size = 100;
        void *array[array_size];
        char **strings;

        if (sig == SIGSEGV)
            fprintf(stderr, "\n###### 段错误 ######\n");
        else if (sig == SIGFPE)
            fprintf(stderr, "\n###### 浮点数例外 ######\n");
        else
            fprintf(stderr, "\n###### 其他错误 ######\n");

        if (1)
        {
            Exception ex("");
            cout << ex.filename() << "\n";
            cout << ex.line() << "\n";
            printf("reason: %s\n", ex.what());
            printf("stack trace: %s\n", ex.stack_trace());
        }
        else
        {
            int size = backtrace(array, array_size);
            fprintf(stderr, "Backtrace (deep : %d):\n\n", size);
            strings = backtrace_symbols(array, size);
            for (int i = 0; i<size; ++i)
            {
                fprintf(stderr, "%d: %s\n", i, strings[i]);
            }
            free(strings);
        }

        exit(-1);
    }

    void test_exception()
    {
        // catch 段错误
        signal(SIGSEGV, test2::debugBacktrace);
        // catch 浮点数例外 
        signal(SIGFPE, test2::debugBacktrace);

        test2::aTest(1);  // 出发异常
    }
}


int main(void)
{
    std::cout << "========== test exception ==========\n";
    test1::test_exception();

    std::cout << "=================================================\n";
    test2::test_exception();

    return 0;
}
