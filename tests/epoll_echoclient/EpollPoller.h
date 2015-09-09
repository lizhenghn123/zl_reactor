#pragma once
#include <sys/epoll.h>
struct epoll_event;

class EpollPoller
{
public:
    explicit EpollPoller(bool one_shot = false, int maxevents = 4096);
    ~EpollPoller();

public:
    int add(int fd);
    int mod(int fd, bool write = true, bool read = false);
    int del(int fd);

    int poll(int timeoutMs);

    const epoll_event* events() const
    {
        return events_;
    }

    const epoll_event& nextEvent(int idx)
    {
        return events_[idx];
    }
    //const epoll_event& operator[](int index)
    //{
    //    return events_[index];
    //}
    //int nextFd()
    //{
    //    return events_[nextEventIdx_++].data.fd;
    //}

private:
    bool one_shot_;
    int epollFd_;
    int maxEvnets_;
    //int nextEventIdx_;
    epoll_event* events_;
};