#include <iostream>
#include <stdio.h>
#include <string.h>
#include "base/Logger.h"
#include "base/LogFile.h"
#include "base/Timestamp.h"
using namespace std;
using namespace zl::base;

void test_log()
{
    LOG_CONSOLE_OUTPUT(true);    
    LOG_SET_DEBUG_MODE;    // == LOG_SET_PRIORITY(ZL_LOG_PRIO_DEBUG);

    LOG_DEBUG("This is a LOG_DEBUG msg!");
    LOG_INFO("This is a LOG_INFO msg!");
    LOG_NOTICE("This is a LOG_NOTICE msg!");
    LOG_WARN("This is a LOG_WARN msg!");
    LOG_ERROR("This is a LOG_ERROR msg!");
    LOG_CRITICA("This is a LOG_CRITICA msg!");
    LOG_ALERT("This is a LOG_ALERT msg!");
    LOG_EMERGENCY("This is a LOG_EMERGENCY msg!");
    
    cout << "####### test_log ########\n";
}

void test_logfile()
{
    LogFile logfile;		// LogFile 需要保证在整个程序运行期间不能析构，且只应该创建一个LogFile对象（SingleTon?）
    logfile.setThreadSafe(false);  // 这里的测试是单线程的，所以可以设置不使用线程（内部不会加锁），否则的话需要设置为多线安全的（默认值）
	
	// 下句可以不用调用，该类对象构造的时候进行了设置（注意资源竞态的可能，所有该对象应该最早创建）
    //LOG_SET_LOGHANDLER(std::bind(&LogFile::dumpLog, &logfile, std::placeholders::_1, std::placeholders::_2));

    LOG_DEBUG("This is a LOG_DEBUG msg!");
    LOG_INFO("This is a LOG_INFO msg!");
    LOG_NOTICE("This is a LOG_NOTICE msg!");
    LOG_WARN("This is a LOG_WARN msg!");
    LOG_ERROR("This is a LOG_ERROR msg!");
    LOG_CRITICA("This is a LOG_CRITICA msg!");
    LOG_ALERT("This is a LOG_ALERT msg!");
    LOG_EMERGENCY("This is a LOG_EMERGENCY msg!");
    cout << "####### test_logfile ########\n";
}

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

const char* data = "Hello 0123456789 abcdefghijklmnopqrstuvwxyz[][1234]";
const size_t max_loop = 1000 * 1000;
const char* prefix = "2015-06-16 20:46:36:822426 [NOTICE] ";

void bench(int type)
{
    LogFile logfile;

    int  total = 0;

    const bool kLongLog = false;
    string longStr(3000, 'X');

    if (type == 0)
    {
        
    }
    else if (type == 1)
    {
        LOG_CONSOLE_OUTPUT(false);
        logfile.setThreadSafe(false); 
        LOG_SET_LOGHANDLER(std::bind(&LogFile::dumpLog, &logfile, std::placeholders::_1, std::placeholders::_2));
    }
    else if (type == 2)
    {
        
    }

    printf("one message size : [%d]\n", (strlen(data) + strlen(prefix)));
    Timestamp start(Timestamp::now());
    for (int i = 0; i < max_loop; ++i)
    {
        LOG_NOTICE(data);
    }
    Timestamp end(Timestamp::now());

    total = max_loop * (strlen(data) + strlen(prefix));
    double seconds = Timestamp::timeDiff(end, start);
    
    const char *typestr = "unknown";
    if (type == 0)
        typestr = "console";
    else if (type == 1)
        typestr = "LogFile";
    else if (type == 2)
        typestr = "AsyncLogger";

    printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n", typestr, seconds, total, max_loop / seconds, total / seconds / (1024 * 1024));
}

int main()
{
    //bench(0);
    test_log();
    test_logfile();
    //test_bench();
    return 0;
}
