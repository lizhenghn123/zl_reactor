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
    LOG_CONSOLE_OUTPUT(true);
    LOG_DEBUG("This is a LOG_DEBUG msg!");
    LOG_INFO("This is a LOG_INFO msg!");
    LOG_NOTICE("This is a LOG_NOTICE msg!");
    LOG_WARN("This is a LOG_WARN msg!");
    LOG_ERROR("This is a LOG_ERROR msg!");
    LOG_CRITICA("This is a LOG_CRITICA msg!");
    LOG_ALERT("This is a LOG_ALERT msg!");
    LOG_EMERGENCY("This is a LOG_EMERGENCY msg!");
    LOG_NOTICE("###############");
    cout << __FILE__ << "\n";

    //test_bench();
    return 0;
}
