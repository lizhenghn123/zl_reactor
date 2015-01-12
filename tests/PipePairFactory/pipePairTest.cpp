#include <stdio.h>
#include <sys/wait.h>
#include "net/PipePairFactory.h"
using namespace zl::net;

#define MAX_LOOP 5

template < typename Factory>
void test_pipe()
{
    Factory ppf;
	
	pid_t pid = fork();
    if(pid > 0)     //Parent process
    {
        ppf.closeRead();

        int val = 0;
        while(val < MAX_LOOP)
        {
            sleep(1);
            ++val;
            printf("parent : send data: %d\n", val);
            ppf.write((const char *)&val, sizeof(val));
        }
    }
    else    // Child process
    {
        ppf.closeWrite();
        int val = 0;
        while(val < MAX_LOOP)
        {
            int ret = ppf.read((char *)&val, sizeof(val));
			if(ret > 0)
				printf("child  : recv data: %d\n", val);
			else
			{
				if(errno == EAGAIN)
					continue;
				else
					break;
			}
        }
    }

	waitpid( pid, NULL, 0 );
	printf("game over\n");
}

template < typename Factory>
void test_fullDuplex() 
{
    Factory ppf;
	
	pid_t pid = fork();
    if(pid > 0)     //Parent process
    {
        //ppf.closeRead();

        int val = 0;
        while(val < MAX_LOOP)
        {
            sleep(1);
            ++val;
            printf("parent : send data: %d\n", val);
            ppf.write((const char *)&val, sizeof(val));
			sleep(1);
			val = -1;
			int ret = ppf.read((char *)&val, sizeof(val));
			if(ret > 0)
				printf("parent : recv data: %d\n", val);
			else
			{
				printf("parent : read error [%d][%s]\n", errno, strerror(errno));
				if(errno == EAGAIN)
					continue;
				else
					break;
			}
        }
		printf("parent over\n");
    }
    else    // Child process
    {
        //ppf.closeWrite();
        int val = 0;
        while(val < MAX_LOOP)
        {
            int ret = ppf.read((char *)&val, sizeof(val));
			if(ret > 0)
				printf("child  : recv data: %d\n", val);
			else
			{
				//printf("child  : read error [%d][%s]\n", errno, strerror(errno));
				if(errno == EAGAIN)
					continue;
				else
					break;
			}
			val *= 2;
			printf("child  : send data: %d\n", val);
            ppf.write((const char *)&val, sizeof(val));
			sleep(1);
        }
		printf("child  over\n");
    }

	waitpid( pid, NULL, 0 );
	printf("game over\n");
}

template < typename Factory>
void test_pipeNotify()
{
	Factory ppf;
	
	pid_t pid = fork();
    if(pid > 0)     //Parent process
    {
		printf("parent : wait child process do job\n");
		ppf.wait(-1);
		printf("parent : my child process done\n");
    }
    else    // Child process
    {
		// do something else...
		printf("child  : starting do something\n");
		sleep(3);
		ppf.notify();
		printf("child  : notify father process job is done\n");
    }

	waitpid( pid, NULL, 0 );
	printf("game over\n");
}

#define TEST_CASE 3
int main(int argc, char *argv[])
{
#if TEST_CASE == 1
	printf("test PipePairFactory\n");
	test_pipe<zl::net::PipePairFactory>();
	
#elif 	TEST_CASE == 2
	printf("test SocketPairFactory\n");
	test_pipe<zl::net::SocketPairFactory>();

#elif 	TEST_CASE == 3
	printf("test TcpPairFactory\n");
	test_pipe<zl::net::TcpPairFactory>();
	
#elif 	TEST_CASE == 4
	printf("test fullDuplexByPipe\n");
	test_fullDuplex<zl::net::PipePairFactory>();
	
#elif 	TEST_CASE == 5	
	printf("test fullDuplexByPipe\n");
	test_fullDuplex<zl::net::SocketPairFactory>();
	
#elif 	TEST_CASE == 6	
	printf("test fullDuplexByPipe\n");
	test_fullDuplex<zl::net::TcpPairFactory>();
	
#elif 	TEST_CASE == 7
	printf("test PipePairNotifyByPipe\n");
	test_pipeNotify<zl::net::PipePairFactory>();
	
#elif 	TEST_CASE == 8
	printf("test PipePairNotifyByPipe\n");
	test_pipeNotify<zl::net::SocketPairFactory>();	
	
#elif 	TEST_CASE == 9
	printf("test PipePairNotifyByPipe\n");
	test_pipeNotify<zl::net::TcpPairFactory>();		
#endif	
    return 0;
}
