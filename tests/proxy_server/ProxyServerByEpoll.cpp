#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>
using namespace std;

/// 简单的代理服务器(中转服务器)
/// 通过io复用方式将所有客户端连接放在同一个线程中与真正的server进行通信
/// 本代理服务器不改变任何数据流，也即是传说中的透传

void parse_args(int argc, char **argv);
void proxy();
void print_exit(const char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}

int kListenPort;                            //本地监听端口
struct sockaddr_in kRemoteServerAddr;       //被代理服务器的地址

int main(int argc, char **argv)
{
    parse_args(argc, argv);

    proxy();

    return 0;
}

void parse_args(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "usage: %s <proxy-port> <host ip> <service-name|port-number> \r\n", argv[0]);
        exit(1);
    }

    struct
    {
        char listenPort[16];
        char isolate_host[64];
        char service_name[32];
    }pargs;

    strcpy(pargs.listenPort, argv[1]);
    strcpy(pargs.isolate_host, argv[2]);
    strcpy(pargs.service_name, argv[3]);
    for (size_t i = 0; i < strlen(pargs.listenPort); i++)
    {
        if (!isdigit(*(pargs.listenPort + i)))
        {
            printf("invalid proxy port : %s\r\n", pargs.listenPort);
            exit(0);
        }
    }
    kListenPort = atoi(pargs.listenPort);

    // 设置被代理服务器的ip和端口
    bzero(&kRemoteServerAddr, sizeof(kRemoteServerAddr));
    kRemoteServerAddr.sin_family = AF_INET;

    struct hostent *hostp;
    unsigned long inaddr = ::inet_addr(pargs.isolate_host);
    if (inaddr != INADDR_NONE)
    {
        bcopy(&inaddr, &kRemoteServerAddr.sin_addr, sizeof(inaddr));
    }
    else if ((hostp = ::gethostbyname(pargs.isolate_host)) != NULL)
    {
        bcopy(hostp->h_addr, &kRemoteServerAddr.sin_addr, hostp->h_length);
    }
    else
    {
        printf("%s unknow host \r\n", pargs.isolate_host);
        exit(1);
    }

#define TCP_PROTO "tcp"

    struct servent *servp;
    if ((servp = ::getservbyname(pargs.service_name, TCP_PROTO)) != NULL)
    {
        kRemoteServerAddr.sin_port = servp->s_port;
    }
    else if (::atoi(pargs.service_name) > 0)
    {
        kRemoteServerAddr.sin_port = htons(atoi(pargs.service_name));
    }
    else
    {
        printf("%s invalid unknow service or port\n", pargs.service_name);
        exit(1);
    }

    printf("listen port : %d, remote server : (%s:%s)\n\n", kListenPort, pargs.isolate_host, pargs.service_name);
}

int set_noblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0)   print_exit("error fcntl\n");

    int ret = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    return ret;
}

int set_reuseaddr(int fd)
{
    int optval = 1;
    return ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

int create_server_fd()
{
    int serverfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd <= 0)
    {
        printf("failed to crate server socket");
        exit(0);
    }

    set_reuseaddr(serverfd);

    struct sockaddr_in servaddr;
    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(kListenPort);
    int ret = ::bind(serverfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        printf("failed to bind server socket to specified");
        exit(1);
    }

    ::listen(serverfd, 1024);

    return serverfd;
}

int connect_remote_server(int clientfd)
{
    int remotefd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (remotefd < 0)
    {
        print_exit("failed to create socket to host");
    }

    int connstat = ::connect(remotefd, (struct sockaddr *) &kRemoteServerAddr, sizeof(kRemoteServerAddr));
    switch (connstat)
    {
    case 0:
        break;
    case ETIMEDOUT:
    case ECONNREFUSED:
    case ENETUNREACH:       // 连接不上被代理服务器时就关闭client
    {
        const char* errormsg = "cannot connect the server";
        ::write(clientfd, errormsg, strlen(errormsg));
        ::close(clientfd);
        exit(1);
        break;
    }
    default:
        print_exit("failed to connect to host");
        return -1;
    }
    return remotefd;
}

#define JUST_BREAK_IF_FAILURE(ret)     if (ret <= 0)  break

#define MAX_EPOLL_EVENTS  512
#define MAX_IO_BUF_LEN    2048

void proxy()
{
    int listenfd = create_server_fd();
    assert(listenfd > 0);
    set_noblock(listenfd);

    int epollfd = ::epoll_create1(0);
    assert(epollfd > 0);

    struct epoll_event ev = { 0, 0 };
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    ::epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

    printf("proxy server start, and listening on %d......\n", kListenPort);

    map<int, int> sockfd_map;
    struct epoll_event* pevents = (struct epoll_event*)malloc(MAX_EPOLL_EVENTS * sizeof(struct epoll_event));
    for (;;)    // event loop
    {
        int ndfs = ::epoll_wait(epollfd, pevents, MAX_EPOLL_EVENTS, -1);

        //处理所发生的所有事件 
        for (int i = 0; i < ndfs; i++)
        {
            // an error on this fd or not read read
            if ((pevents[i].events & EPOLLERR) || (pevents[i].events & EPOLLHUP) ||
                (!(pevents[i].events & EPOLLIN)))
            {
                perror("epoll_wait: ");
                ::close(pevents[i].data.fd);
            }
            else if (pevents[i].data.fd == listenfd)    // 监听到有新的连接
            {
                struct sockaddr_in clientaddr;
                socklen_t cli_len = sizeof(clientaddr);
                for (;;)
                {
                    int clientfd = ::accept(listenfd, (struct sockaddr*)&clientaddr, &cli_len);
                    if (clientfd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)  // 接收了所有的client
                        {
                            break;
                        }
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    printf("get one client : %d\n", clientfd);
                    int ret = set_noblock(clientfd);

                    int remotefd = connect_remote_server(clientfd);
                    if (remotefd < 0)    print_exit("connect isolate error");

                    ret = set_noblock(remotefd);

                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN | EPOLLET;
                    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                    if (ret < 0)         print_exit("epoll ctl error");

                    ev.data.fd = remotefd;
                    ev.events = EPOLLIN | EPOLLET;
                    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, remotefd, &ev);
                    if (ret < 0)         print_exit("epoll ctl error");

                    sockfd_map[clientfd] = remotefd;
                    sockfd_map[remotefd] = clientfd;
                }
            }
            else
            {
                bool closeit = false;
                while (1)
                {
                    char buf[MAX_IO_BUF_LEN];
                    //read the send data 
                    ssize_t count = ::read(pevents[i].data.fd, buf, sizeof(buf));
                    if (count == -1)
                    {
                        if (errno != EAGAIN)
                        {
                            perror("read: ");
                            closeit = true;
                        }
                        break;
                    }
                    else if (count == 0)
                    {
                        closeit = true;
                        break;
                    }

                    printf("recv data %ld on %d\n", count, pevents[i].data.fd);
                    ssize_t ret = ::write(sockfd_map[pevents[i].data.fd], buf, count);
                    if (ret == -1)
                    {
                        perror("write");
                        closeit = true;
                        break;
                    }
                    else if (count == 0)
                    {
                        closeit = true;
                        break;
                    }
                }

                if (closeit)
                {
                    fprintf(stderr, "close link on (%d--%d)\n", pevents[i].data.fd, sockfd_map[pevents[i].data.fd]);
                    ev.data.fd = pevents[i].data.fd;
                    ev.events = 0;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, pevents[i].data.fd, &ev);
                    ev.data.fd = sockfd_map[pevents[i].data.fd];
                    ev.events = 0;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd_map[pevents[i].data.fd], &ev);
                    ::close(pevents[i].data.fd);
                    ::close(sockfd_map[pevents[i].data.fd]);
                    sockfd_map.erase(pevents[i].data.fd);
                    sockfd_map.erase(sockfd_map[pevents[i].data.fd]);
                }
            }
        }
    }
    free(pevents);
}
