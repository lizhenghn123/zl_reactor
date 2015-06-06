/*************************************************************************
	File Name   : backtrace_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年06月02日 星期二 08时56分59秒
 ************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>

int test_flag = 1;  // 1 means SIGSEGV, 2 means SIGFPE

namespace detail
{
    void c_do_nothing(float f, int c)
    {
        printf("This is c_do_nothing : %f, %d\n", f, c);
        if(test_flag == 1)
        {
            char *p = (char*)c;
            *p = 'C';     // 段错误
        }
        else if(test_flag == 2)
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
    bTwo(a+1, "hello world");
}

void debugBacktrace(int sig)
{
    const static int array_size = 100;
    void *array[array_size];
    char **strings;
    
    if(sig == SIGSEGV)
        fprintf(stderr, "\n###### 段错误 ######\n");
    else if(sig == SIGFPE)
        fprintf(stderr, "\n###### 浮点数例外 ######\n");
    else
        fprintf(stderr, "\n###### 其他错误 ######\n");

    int size = backtrace(array, array_size);
    fprintf(stderr, "Backtrace (deep : %d):\n", size);
    strings = backtrace_symbols(array, size);
    for(int i=0; i<size; ++i)
    {
        fprintf(stderr, "%d: %s\n", i, strings[i]);
    }
    free(strings);
    exit(-1);
}

int main()
{
    // catch 段错误
    signal(SIGSEGV, debugBacktrace);
    // catch 浮点数例外 
    signal(SIGFPE, debugBacktrace);

    aTest(1);

    return 0;
}

/***  输出结果 
This is aTest : 1
This is bTwo : 2  hello world
This is c_do_nothing : 2.400000, 4

###### Segmentation fault ######
Backtrace (deep : 8):
0: ./backtrace_test(_Z14debugBacktracei+0x43) [0x400d14]
1: /lib64/libc.so.6() [0x309ce32920]
2: ./backtrace_test(_ZN6detail12c_do_nothingEfi+0x39) [0x400c39]
3: ./backtrace_test(_Z4bTwoiPKc+0x4a) [0x400c9c]
4: ./backtrace_test(_Z5aTesti+0x31) [0x400ccf]
5: ./backtrace_test(main+0x1d) [0x400dce]
6: /lib64/libc.so.6(__libc_start_main+0xfd) [0x309ce1ecdd]
7: ./backtrace_test() [0x400ac9]

说明：
1.当出发sigsegv时，程序正常流程被打断，信号处理函数debugBacktrace被调用，对应到结果输出中的标号0，1两行；
2.每行调用栈输出中的最后[]表示被调用函数的返回地址
3.从输出中可以看出最后一次的正常函数调用是[0x400c39], 可以进行反汇编：
    objdump -D backtrace_test > backtrace_test.dis
    然后就可以从汇编文件中找到相应函数地址了；
4.另外，也可以通过c++filt命令即可解析出原函数名，比如c++filt  _Z14debugBacktracei或者 c++filt _ZN6detail12c_do_nothingEfi；
***/
