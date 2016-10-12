#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include "zlreactor/base/MasterWorkerProcess.h"
using namespace zl;

class Test
{
public:
    Test() : num_(0) {}

    int num_;
};

//#define TEST_SUB_PROCESS_EXIT

void child_process_work(zl::MasterWorkerProcess* main_process, int jobid, void* arg)
{
    assert(main_process->pid() == MasterWorkerProcess::mainProcessPid());
    assert(MasterWorkerProcess::mainProcessPid() != ::getpid());

	FILE* file = fopen("one.log", "a+");
	assert(file);
    while (!main_process->shutdown())
    {
		char buffer[1024];
		sprintf(buffer, "[%d][%d][%d]=======[%d]======\n", main_process->pid(), MasterWorkerProcess::mainProcessPid(), ::getpid(), main_process->shutdown());
        printf("%s", buffer);
		fwrite(buffer, strlen(buffer), 1, file);
		fflush(file);
        sprintf(buffer, "child process[%d], jobid[%d], just sleep 6s\n", ::getpid(), jobid);
		printf("%s", buffer);
		fwrite(buffer, strlen(buffer), 1, file);
		fflush(file);
        ::sleep(6);
    #if defined(TEST_SUB_PROCESS_EXIT)
        break;
    #endif
    }
    printf("child process[%d] [%d]: exit the work\n", ::getpid(), main_process->shutdown());
	fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("error, should run as : bin child_num, ex : %s 3\n", argv[0]);
        return 0;
    }

    printf("main process[%d] starting...\n", ::getpid());

    zl::MasterWorkerProcess process;
    process.createWorkProcess(atoi(argv[1]), child_process_work, NULL);

    return 0;
}
