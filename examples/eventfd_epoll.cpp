#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

//eventfd与pipe类似，可以用来完成两个线程之间事件触发；甚至进程间消息响应
//eventfd优点：
//1. 比pipe少用了一个文件描述符;
//2. eventfd是固定大小缓冲区（8 bytes），而pipe是可变大小的

int g_eventfd = -1;

void *read_thread(void *dummy)
{
    int ret = 0;
    uint64_t count = 0;
    int epoll_fd = -1;
    struct epoll_event events[10];

    if (g_eventfd < 0)
    {
        printf("g_eventfd not inited.\n");
        return NULL;
    }

    epoll_fd = epoll_create(1024);
    if (epoll_fd < 0)
    {
        perror("epoll_create fail: ");
        return NULL;
    }

    {
        struct epoll_event read_event;

        read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
        read_event.data.fd = g_eventfd;

        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, g_eventfd, &read_event);
        if (ret < 0)
        {
            perror("epoll ctl failed:");
            return NULL;
        }
    }

    while (1)
    {
        ret = epoll_wait(epoll_fd, &events[0], 10, 5000);
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
                    int event_fd = events[i].data.fd;
                    ret = ::read(event_fd, &count, sizeof(count));
                    if (ret < 0)
                    {
                        perror("read fail:");
                        break;
                    }
                    else
                    {
                        struct timeval tv;
                        gettimeofday(&tv, NULL);
                        printf("success read from g_eventfd, read %d bytes(%llu) at %lds %ldus\n",
                               ret, count, tv.tv_sec, tv.tv_usec);
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

    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t pid = 0;
    uint64_t count = 0;
    int ret = 0;
    int i = 0;

    g_eventfd = eventfd(0, 0);
    if (g_eventfd < 0)
    {
        perror("eventfd failed.");
        goto fail;
    }

    ret = pthread_create(&pid, NULL, read_thread, NULL);
    if (ret < 0)
    {
        perror("pthread create:");
        goto fail;
    }

    for (i = 0; i < 5; i++)
    {
        count = i;
        ret = ::write(g_eventfd, &count, sizeof(count));
        if (ret < 0)
        {
            perror("write event fd fail:");
            goto fail;
        }
        else
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            printf("success write to g_eventfd, write %d bytes(%llu) at %lds %ldus\n",
                   ret, count, tv.tv_sec, tv.tv_usec);
        }

        sleep(1);
    }

fail:
    if (0 != pid)
    {
        pthread_join(pid, NULL);
        pid = 0;
    }

    if (g_eventfd >= 0)
    {
        close(g_eventfd);
        g_eventfd = -1;
    }
    return ret;
}
