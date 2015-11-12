#include "EpollPoller.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

EpollPoller::EpollPoller(bool one_shot/* = false*/, int maxevents/* = 4096*/)
{
    one_shot_ = one_shot;
    maxEvnets_ = maxevents;
    epollFd_ = ::epoll_create(maxevents);
    events_ = new epoll_event[maxevents];
    assert(epollFd_ > 0);
    assert(events_);
}

EpollPoller::~EpollPoller()
{
    ::close(epollFd_);
    delete[] events_;
}


int EpollPoller::add(int fd)
{
    struct epoll_event ev = { 0, { 0 } };
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP/* | EPOLLET*/;
    if (one_shot_)
        ev.events |= EPOLLONESHOT;
    return ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);
}

int EpollPoller::mod(int fd, bool write/* = true*/, bool read/* = false*/)
{
    struct epoll_event ev = { 0, { 0 } };
    ev.data.fd = fd;
    ev.events = EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    if (one_shot_)
        ev.events |= EPOLLONESHOT;
    if (write)
        ev.events |= EPOLLOUT;
    if (read)
        ev.events |= EPOLLIN;

    return ::epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
}

int EpollPoller::del(int fd)
{
    struct epoll_event ev = { 0, { 0 } };
    ev.data.fd = fd;
    return ::epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev);
}

int EpollPoller::poll(int timeoutMs)
{
    //nextEventIdx_ = 0;
    return ::epoll_wait(epollFd_, events_, maxEvnets_, timeoutMs);
}
