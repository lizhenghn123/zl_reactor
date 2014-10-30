#include "net/EpollPoller.h"
#include <string.h>
#include <sys/epoll.h>
#include "net/Socket.h"
//#include "Channel.h"

NAMESPACE_ZL_NET_START


EpollPoller::EpollPoller(ZL_SOCKET listenfd, int event_size /*= MAX_EPOLL_EVENTS*/, bool enableET/* = false*/)
{
    listenfd_ = listenfd;
    enableET_ = enableET;
    epollfd_ = epoll_create(event_size);

    //    struct epoll_event ev;
    //    ev.data.fd = srvSocket_.GetSocket(); //设置与要处理的事件相关的文件描述符
    //    ev.events = EPOLLIN|EPOLLET;         //设置要处理的事件类型
    //    //ev.events=EPOLLIN;
    //
    //    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, srvSocket_.GetSocket(), &ev);  //注册epoll事件
}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

/*
 * 增加Socket到事件中
 *
 * @param socket 被加的socket
 * @param enableRead: 设置是否可读
 * @param enableWrite: 设置是否可写
 * @return  操作是否成功, true – 成功, false – 失败
 */
bool EpollPoller::updateChannel(Channel *channel/*, bool enableRead, bool enableWrite*/)
{
/*    channel->ClearChannel();
    ZL_SOCKET sock = channel->GetSocket()->GetSocket();

    struct epoll_event ev = { 0, { 0 } };
    //ev.data.ptr = channel;
    ev.data.fd = sock;

    ev.events = 0;
    if(enableRead)
        ev.events |= EPOLLIN;
    if(enableWrite)
        ev.events |= EPOLLOUT;
    if(enableET_)
        ev.events |= EPOLLET;

    ev.events |= EPOLLERR;
    ev.events |= EPOLLHUP;

    channelMap[sock] = channel;

    bool rc = (epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->GetSocket()->GetSocket(), &ev) == 0);
    return rc; */
	return true;
}

/*
 * 删除Socket到事件中
 *
 * @param socket 被删除socket
 * @return  操作是否成功, true – 成功, false – 失败
 */
bool EpollPoller::removeChannel(Channel *channel)
{
/*    channel->ClearChannel();
    ZL_SOCKET sock = channel->GetSocket()->GetSocket();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = 0;
    ev.data.fd = 0;//channel->GetSocket()->GetSocket();
    ev.events = 0;

    ChannelMap::iterator itr = channelMap.find(sock);
    if(itr != channelMap.end())
    {
        channelMap.erase(itr);
    }

    bool rc = (epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->GetSocket()->GetSocket(), &ev) == 0);
    return rc;  */
	return true;
}

/*
 * 得到读写事件。
 *
 * @param timeout  超时时间(单位:ms)
 * @param activeSockets  激活事件列表
 * @param cnt   events的数组大小
 * @return 事件数, 0为超时, -1为出错了
 */
Timestamp EpollPoller::poll(int timeout, ChannelList& activeChannels)
{
	return Timestamp::now();
/*    struct epoll_event epoll_events[MAX_EPOLL_EVENTS];
    int nfds = epoll_wait(epollfd_, epoll_events, MAX_EPOLL_EVENTS, -1); //等待epoll事件的发生
    Channel *channel;
    ZL_SOCKET sock;
    uint32_t events;
    for(int i = 0; i < nfds; ++i)   //处理所发生的所有事件
    {
        events = epoll_events[i].events;
        sock = epoll_events[i].data.fd;
        //channel = epoll_events[i].data.ptr;
        channel = getChannel(sock);
        if(channel == NULL)
        {
            printf("channel == null[%d]\n", sock);
            continue;
        }
        channel->clearChannel();
        if(sock == listenfd_)
        {
            channel->setChannel(SOCKETEVENT_ACCEPT);
        }
        else if(events & EPOLLIN)
        {
            channel->setChannel(SOCKETEVENT_READ);
        }
        else if(events & EPOLLOUT)
        {
            channel->setChannel(SOCKETEVENT_WRITE);
        }
        else if((events & EPOLLRDHUP) || (events & EPOLLERR))
        {
            channel->setChannel(SOCKETEVENT_HUP | SOCKETEVENT_ERROR);
            printf("EPOLLRDHUP[%d]\n", channel->GetChannel());
            //            struct epoll_event ev;
            //            ev.data.ptr = 0;
            //            epoll_ctl(epollfd_, EPOLL_CTL_DEL, sock, &ev);
        }
        if(channel->getChannel())
            activeChannels.push_back(channel);
    }
    return activeChannels.size();
*/
}

NAMESPACE_ZL_NET_END
