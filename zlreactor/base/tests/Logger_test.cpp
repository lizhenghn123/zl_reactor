#include <iostream>
#include <stdio.h>
#include <string.h>
#include "base/Logger.h"
#include "base/LogFile.h"
using namespace std;
using namespace zl::base;

void test_log()
{
    LOG_CONSOLE_OUTPUT(false);
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

typedef std::function<void(const char* , size_t )> FUNC;

void test_logfile()
{
    LogFile logfile;
    logfile.setThreadSafe(false);
    LOG_SET_LOGHANDLER(std::bind(&LogFile::dumpLog, &logfile, std::placeholders::_1, std::placeholders::_2));
  //  zl::base::Logger::setLogHandler(std::bind(&LogFile::dumpLog, &logfile));
    //FUNC f = std::bind(&LogFile::dumpLog, &logfile, std::placeholders::_1, std::placeholders::_2);
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

int main()
{
    //test_log();
    test_logfile();
    //test_bench();
    return 0;
}
