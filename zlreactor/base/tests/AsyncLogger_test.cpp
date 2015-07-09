#include <iostream>
#include <stdio.h>
#include <string.h>
#include "base/Logger.h"
#include "base/LogFile.h"
#include "base/Timestamp.h"
#include "base/AsyncLogger.h"
#include "thread/Thread.h"
#include <unistd.h>
using namespace std;
using namespace zl::base;
using namespace zl::thread;

void test_asynclogger()
{
    AsyncLogger logger;
    logger.start();

    //LOG_CONSOLE_OUTPUT(false);
    LOG_DEBUG("This is a LOG_DEBUG msg!");
    LOG_INFO("This is a LOG_INFO msg!");
    LOG_NOTICE("This is a LOG_NOTICE msg!");
    LOG_WARN("This is a LOG_WARN msg!");
    LOG_ERROR("This is a LOG_ERROR msg!");
    LOG_CRITICA("This is a LOG_CRITICA msg!");
    LOG_ALERT("This is a LOG_ALERT msg!");
    LOG_EMERGENCY("This is a LOG_EMERGENCY msg!");
   // logger.stop();
    cout << "####### test_log ########\n";
}

void producer()
{
    for (int i = 0; i < 5000; ++i)
    {
        LOG_DEBUG("[%d] This is a LOG_DEBUG msg!", this_thread::get_id().value());
        LOG_INFO("[%d] This is a LOG_INFO msg!", this_thread::get_id().value());
        LOG_NOTICE("[%d] This is a LOG_NOTICE msg!", this_thread::get_id().value());
        LOG_WARN("[%d] This is a LOG_WARN msg!", this_thread::get_id().value());
        LOG_ERROR("[%d] This is a LOG_ERROR msg!", this_thread::get_id().value());
        LOG_CRITICA("[%d] This is a LOG_CRITICA msg!", this_thread::get_id().value());
        LOG_ALERT("[%d] This is a LOG_ALERT msg!", this_thread::get_id().value());
        LOG_EMERGENCY("[%d] This is a LOG_EMERGENCY msg!", this_thread::get_id().value());
    }
}

void test_bench()
{
    AsyncLogger logger;
    LOG_SET_LOGHANDLER(std::bind(&AsyncLogger::output, &logger, std::placeholders::_1, std::placeholders::_2));
    logger.start();

    int max_threads = 10;
    vector<Thread*> vec;
    for (int i = 0; i < max_threads; ++i)
    {
        vec.push_back(new Thread(producer));
    }

    for (int i = 0; i < max_threads; ++i)
    {
        vec[i]->join();
        delete vec[i];
    }
    cout << "####### test_bench ########\n";
}

int main()
{
    test_asynclogger();
    //test_bench();
    return 0;
}
