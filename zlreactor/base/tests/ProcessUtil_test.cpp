#include <stdio.h>
#include "zlreactor/base/ProcessUtil.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
using namespace zl;
using namespace zl::base;

void test_CurrentProcess()
{
    printf("--------------------------------- test_CurrentProcess ---------------------------------\n");
    printf("====pid==== %d\n", zl::ProcessUtil::pid());
    printf("====uid==== %d\n", zl::ProcessUtil::uid());
    printf("====euid==== %d\n", zl::ProcessUtil::euid());
    printf("====procname==== %s\n", zl::ProcessUtil::procname().c_str());

    printf("====start time==== %s\n", zl::ProcessUtil::startTime().toString().c_str());
    printf("====elapsed time==== %ld\n", zl::ProcessUtil::elapsedTime());
    printf("====hostname==== %s\n", zl::ProcessUtil::hostname().c_str());
    printf("====username==== %s\n", zl::ProcessUtil::username().c_str());

    printf("====status====\n%s\n", zl::ProcessUtil::procStatus().c_str());
    printf("====stat==== %s\n", zl::ProcessUtil::procStat().c_str());
    printf("====thread stat==== %s\n", zl::ProcessUtil::threadStat().c_str());
    printf("====exePath==== %s\n", zl::ProcessUtil::exePath().c_str());

    printf("====opened files==== %d\n", zl::ProcessUtil::openedFiles());
    printf("====man open files==== %d\n", zl::ProcessUtil::maxOpenFiles());

    printf("====threads==== %zd\n", zl::ProcessUtil::threads().size());
    printf("====num threads==== %d\n", zl::ProcessUtil::numThreads());

    sleep(3);
    printf("====elapsed time==== %ld\n", zl::ProcessUtil::elapsedTime());
}

void test_OtherProcess(const char* procname)
{
    printf("--------------------------------- test_OtherProcess ---------------------------------\n");
    int pid = zl::ProcessUtil::getPidByName(procname);
    printf("====pid==== %d\n", pid);
    printf("====procname==== %s\n", zl::ProcessUtil::getNameByPid(pid).c_str());
    printf("====exePath==== %s\n", zl::ProcessUtil::exePath(pid).c_str());

    printf("====status====\n%s\n", zl::ProcessUtil::procStatus(pid).c_str());
    printf("====stat==== %s\n", zl::ProcessUtil::procStat(pid).c_str());
    //printf("====thread stat==== %s\n", zl::ProcessUtil::threadStat(pid).c_str());

    printf("====threads==== %zd\n", zl::ProcessUtil::threads(pid).size());
    printf("====num threads==== %d\n", zl::ProcessUtil::numThreads(pid));
}

int main(int argc, char* argv[])
{
    //test_CurrentProcess();

    test_OtherProcess("HttpInspectorServer");

    return 0;
}
