#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <errno.h>

void test_signalfd();
void test_signalfd_epoll();

int main(int argc, char *argv[])
{
    //test_signalfd();
	test_signalfd_epoll();
}

void test_signalfd()
{
    sigset_t mask;
    struct signalfd_siginfo fdsi;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* 阻塞信号以使得它们不被默认的处理试方式处理 */
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        perror("sigprocmask");

    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
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
		exit(EXIT_SUCCESS);
	}

    for (;;) 
	{
        ssize_t s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
        if (s != sizeof(struct signalfd_siginfo))
            perror("read");

        if (fdsi.ssi_signo == SIGINT)        // ctrl + 'c'
		{
            printf(" Got SIGINT\n");
        } 
		else if (fdsi.ssi_signo == SIGQUIT)  // ctrl + '\'
		{
            printf(" Got SIGQUIT\n");
            exit(EXIT_SUCCESS);
        } 
		else 
		{
            printf("Read unexpected signal\n");
        }
    }
}

void test_signalfd_epoll()
{
    sigset_t mask; 
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* 阻塞信号以使得它们不被默认的处理试方式处理 */
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        perror("sigprocmask");

    int sfd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (sfd == -1)
    {
        perror("signalfd");
		exit(EXIT_SUCCESS);
	}

	int epoll_fd = epoll_create(1024);	

	struct epoll_event read_event;
	read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
	read_event.data.fd = sfd;
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &read_event);
	if (ret < 0)
	{
		perror("epoll ctl failed:");
		exit(EXIT_SUCCESS);
	}

	const static int max_events = 10;
	struct epoll_event events[max_events];
    while (1)
    {
        int ret = epoll_wait(epoll_fd, &events[0], max_events, 5000);
        if (ret > 0)
        {
            for (int i = 0; i < ret; i++)
            {
                if (events[i].events & EPOLLHUP)
                {
                    printf("epoll eventfd has epoll hup.\n");
                    break;
                }
                else if (events[i].events & EPOLLERR)
                {
                    printf("epoll eventfd has epoll error.\n");
                    break;
                }
                else if (events[i].events & EPOLLIN)
                {
                    int signal_fd = events[i].data.fd;
					struct signalfd_siginfo fdsi;
					ssize_t s = read(signal_fd, &fdsi, sizeof(struct signalfd_siginfo));
					if (s != sizeof(struct signalfd_siginfo))
						perror("read");
					if (fdsi.ssi_signo == SIGINT)        // ctrl + 'c'
					{
						printf(" epoll : Got SIGINT\n");
					} 
					else if (fdsi.ssi_signo == SIGQUIT)  // ctrl + '\'
					{
						printf(" epoll : Got SIGQUIT\n");
						exit(EXIT_SUCCESS);
					} 
					else 
					{
						printf("Read unexpected signal\n");
					}
                }
            }
        }
        else if (ret == 0)
        {
            printf("epoll wait timed out.\n");
            continue;
        }
        else
        {
            perror("epoll wait error:");
            break;
        }
    }

    if (epoll_fd >= 0)
    {
        close(epoll_fd);
        epoll_fd = -1;
    }
}
/*这个例子只是很简单的说明了使用signalfd的方法，并没有真正发挥它的作用，有了这个API，就可以将信号处理作为IO看待，
每一个信号集合（或者某一个对应的信号）就会有对应的文件描述符，这样将信号处理的流程大大简化，将应用程序中的业务作为文件来操作，也体现了linux下的一切皆文件
的说法，非常好，假如有很多种信号等待着处理，每一个信号描述符对待一种信号的处理，那么就可以将信号文件描述符设置为非阻塞，同时结合epoll使用，对信号的
处理转化为IO复用，和这个有相似之处的API还有timerfd*/
