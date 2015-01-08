/*************************************************************************
	File Name   : test_signalfd/TestSignalFd.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年01月07日 星期三 15时39分22秒
 ************************************************************************/

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "net/EventLoop.h"
#include "net/SignalHandler.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void signal_handler(int sig)
{
	switch(sig)
	{
	case SIGINT:   // ctrl + 'c'
		printf("main : Got SIGINT[%d]\n", sig);
		break;
	case SIGHUP: 
		printf("main : Got SIGHUP[%d]\n", sig);
		break;
	case SIGQUIT: // ctrl + '\'
		printf("main : Got SIGQUIT[%d]\n", sig);
		exit(0);
		break;
	case SIGSEGV: 
		printf("main : Got SIGSEGV[%d]\n", sig);
		break;	
	case SIGPIPE: 
		printf("main : Got SIGPIPE[%d]\n", sig);
		break;
	case SIGALRM: 
		printf("main : Got SIGALRM[%d]\n", sig);
		break;
	case SIGCHLD: 
		printf("main : Got SIGCHLD[%d]\n", sig);
		break;
	default: 
		printf("main : Got signal[%d]\n", sig);
		break;		
	}
}

int main()
{
	EventLoop loop;	
	SignalHandler sigHandler(&loop);
	
	sigHandler.setSigHandler(SIGINT, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGHUP, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGQUIT, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGSEGV, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGPIPE, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGALRM, std::bind(signal_handler, std::placeholders::_1));
	sigHandler.setSigHandler(SIGCHLD, std::bind(signal_handler, std::placeholders::_1));
    sigHandler.registerAll();	

    sigHandler.removeSig(SIGALRM);

	loop.loop();
    printf("main loop exits");
}


