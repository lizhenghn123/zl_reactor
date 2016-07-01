/*************************************************************************
	File Name   : test_signalfd/TestSignalFd.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年01月07日 星期三 15时39分22秒
 ************************************************************************/
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/Channel.h"
#include "zlreactor/net/Signalfd.h"
#include "zlreactor/base/Logger.h"
#include <sys/epoll.h>
#include <unordered_map>
#include <sys/signalfd.h>
using namespace std;
using namespace zl;
using namespace zl::net;

/******
#include <sys/signalfd.h>
int signalfd(int fd, const sigset_t *mask, int flags);
把信号集与fd关联起来，第一个参数为-1表示新建一个signalfd，不是-1并且是一个合法的signalfd表示向其添加新的信号。

当fd不是-1并且是一个合法的signalfd时，有问题，我还没搞明白。
******/
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
  
void test_signalfd()
{
	LOG_INFO("----------------test_signalfd----------------");
	LOG_INFO("----------------[SIGINT %d][SIGQUIT %d]----------------", SIGINT, SIGQUIT);
	SignalfdHandler sfd;
	sfd.addSigHandler(SIGINT, signal_handler);
	sfd.addSigHandler(SIGQUIT, signal_handler);
	sfd.registerAll(0);   // block

	sfd.wait();  // sync loop-wait
}

class Test_Signalfd
{
public:
	Test_Signalfd(EventLoop *loop)
	{
		loop_ = loop;
		signalfd_ = new SignalfdHandler; 
		signalfd_->addSigHandler(SIGINT, signal_handler);
		signalfd_->addSigHandler(SIGQUIT, signal_handler);
		signalfd_->registerAll(SFD_NONBLOCK | SFD_CLOEXEC);  // must set nonblock

		sfdChannel_ = new Channel(loop, signalfd_->fd());
		sfdChannel_->setReadCallback(std::bind(&Test_Signalfd::handleRead, this));
		sfdChannel_->enableReading();
	}
	~Test_Signalfd()
	{
		sfdChannel_->disableAll();
		delete sfdChannel_;
		delete signalfd_;
	}
	void handleRead()
	{
		int sig = signalfd_->readSig();
	}
	 
private:
	EventLoop       *loop_;
	Channel         *sfdChannel_;
	SignalfdHandler *signalfd_;
};


void test_signalfd_poll()
{
	EventLoop loop;
	Test_Signalfd ts(&loop);
	loop.loop();
}

int main()
{
    //test_signalfd();

    test_signalfd_poll();
}
