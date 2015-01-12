#include "net/SignalHandler.h"
#include <sys/signalfd.h>
#include <signal.h>
#include "net/Channel.h"
#include "base/ZLog.h"
NAMESPACE_ZL_NET_START

SignalHandler::SignalHandler(EventLoop *loop)
    : loop_(loop), signalFd_(-1), sigFdChannel_(NULL)
{
}

SignalHandler::~SignalHandler()
{
}


void SignalHandler::setSigHandler(int sig, const SignalCallback& handler)
{
	sigHanhlers_[sig] = handler;
}

void SignalHandler::removeSig(int sig)
{
	SigHandlerMap::iterator iter = sigHanhlers_.find(sig);
	if(iter != sigHanhlers_.end())
		sigHanhlers_.erase(iter);
}

bool SignalHandler::haveSignal(int sig)
{
	return sigHanhlers_.find(sig) != sigHanhlers_.end() ? true : false;
}

int SignalHandler::registerAll()
{
    if(signalFd_ > 0)
		return 0;

    sigset_t mask; 
    sigemptyset(&mask);
	for(SigHandlerMap::iterator iter = sigHanhlers_.begin(); iter != sigHanhlers_.end(); ++iter)
	{
		sigaddset(&mask, iter->first);
	}

    // 阻塞信号以使得它们不被默认的处理试方式处理
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
    {
		perror("sigprocmask");
		return -1;
	}

	signalFd_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
	if (signalFd_ == -1)
    {
        perror("signalfd error");
		switch(errno)
		{
		case EBADF:     //fd 文件描述符不是一个有效的文件描述符
		case EINVAL:    //flags 无效； 或者，在 2.6.26 及其前，flags 非零
		case EMFILE:    //达到单个进程打开的文件描述上限
		case ENFILE:    //达到可打开文件个数的系统全局上限 
		case ENODEV:    //不能挂载（内部）匿名结点设备
		case ENOMEM:    //没有足够的内存来创建新的 signalfd 文件描述符
		break;
		}
		return -1;
	}

	sigFdChannel_ = new Channel(loop_, signalFd_);
    sigFdChannel_->setReadCallback(std::bind(&SignalHandler::sigOnRead, this));
	sigFdChannel_->enableReading();

	return 0;
}

void SignalHandler::sigOnRead()
{	
	struct signalfd_siginfo fdsi;
	ssize_t n = ::read(signalFd_, &fdsi, sizeof(struct signalfd_siginfo));
	if (n != sizeof(struct signalfd_siginfo))
	{
		LOG_INFO("SignalHandler::sigOnRead : read signal error[%d][%d]", n, errno);
	}

	int signo = fdsi.ssi_signo;
	if(haveSignal(signo))
	{
		if(sigHanhlers_[signo])
			sigHanhlers_[signo](signo);
		else
			LOG_INFO("SignalHandler::sigOnRead : signal[%d] have not set callback", signo);
	}
	else
	{
		LOG_INFO("SignalHandler::sigOnRead : read unexpected signal[%d]", signo);
	}
}

NAMESPACE_ZL_NET_END
