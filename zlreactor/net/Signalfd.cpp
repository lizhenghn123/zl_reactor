#include "net/Signalfd.h"
#include <signal.h>
#include <assert.h>
#include "net/Channel.h"
#include "base/ZLog.h"

NAMESPACE_ZL_NET_START

SignalfdHandler::SignalfdHandler(int flags/* = SFD_NONBLOCK | SFD_CLOEXEC*/)
    : signalFd_(-1)
{
    isRunning_ = true;

    signalFd_ = createSignalfd(flags);
}

SignalfdHandler::~SignalfdHandler()
{
}

Signalfd SignalfdHandler::createSignalfd(int flags)
{
    sigemptyset(&mask_);
    sigaddset(&mask_, SIGINT);   // 初始化时必须得先添加这个信号，我也没搞明白原因还
    /* 阻塞信号以使得它们不被默认的处理试方式处理 */
    if (sigprocmask(SIG_BLOCK, &mask_, NULL) == -1)
        perror("sigprocmask");

    int  sfd = ::signalfd(-1, &mask_, 0);
    if(sfd < 0)
    {
        LOG_ERROR("signalfd create failure");
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
    }
    return sfd;
}

void SignalfdHandler::addSigHandler(int sig, const SignalCallback& handler)
{
    if(!haveSignal(sig))
    {
        sigaddset(&mask_, sig);
        int r = ::signalfd(signalFd_, &mask_, 0);
        assert(r == signalFd_);

    }
    sigHanhlers_[sig] = handler;
}

void SignalfdHandler::removeSig(int sig)
{
	SigHandlerMap::iterator iter = sigHanhlers_.find(sig);
	if(iter != sigHanhlers_.end())
		sigHanhlers_.erase(iter);
}

bool SignalfdHandler::haveSignal(int sig)
{
	return sigHanhlers_.find(sig) != sigHanhlers_.end() ? true : false;
}

int SignalfdHandler::readSig()
{	
	struct signalfd_siginfo fdsi;
	ssize_t n = ::read(signalFd_, &fdsi, sizeof(struct signalfd_siginfo));
	if (n != sizeof(struct signalfd_siginfo))
	{
		LOG_INFO("SignalfdHandler::readSig : read signal error[%d][%d]", n, errno);
	}

	int signo = fdsi.ssi_signo;
    LOG_INFO("SignalfdHandler::readSig : [%d]", signo);
	if(haveSignal(signo))
	{
		if(sigHanhlers_[signo])
			sigHanhlers_[signo](signo);
		else
			LOG_INFO("SignalfdHandler::readSig : signal[%d] have not set callback", signo);
	}
	else
	{
		LOG_INFO("SignalfdHandler::readSig : read unexpected signal[%d]", signo);
	}
    return signo;
}

void SignalfdHandler::wait()
{
    LOG_INFO("SignalfdHandler::wait()");
	while (isRunning_)
	{
        readSig();
	}
}

NAMESPACE_ZL_NET_END
