#include <iostream>
#include <stdio.h>
#include <string.h>
#include "base/ZLog.h"
using namespace std;
using namespace zl::base;

void test_bench()
{
	char strbuf[4 * 1024];  // 4 * 1024 == MAX_LOG_ENTRY_SIZE
	const char *str = "hello word, zlreactor-c++-net-tcp, over";	
	for(int i = 0; i < 100; i++)
	{
		strncat(strbuf, str, strlen(str));
	}

	printf("Log size of each log entry : %d\n", strlen(strbuf));	

	for(int i = 0; i < 100000000; i++)
	{
		LOG_NOTICE("[value:%d] %s", i, strbuf);
    }
}

int main()
{
    LOG_CONSOLE_OUTPUT(false);
    //LOG_THREAD_SAFE(false);
    LOG_NOTICE("2334444");
    LOG_NOTICE("###############");
    cout << __FILE__ << "\n";
    //test_bench();
    return 0;
}
