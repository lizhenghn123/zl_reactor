#include <stdio.h>
#include "base/ProcessUtil.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
using namespace zl;
using namespace zl::base;

int main(int argc, char* argv[])
{
    if(argc > 1)
    {
        for(int i=0; i<argc; ++i)
            printf("%s\t", argv[i]);
        printf("\n");
    }

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

    getchar();
    return 0;
}
