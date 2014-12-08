#ifndef ZL_EPOLLSERVER_H
#define ZL_EPOLLSERVER_H
#include <pthread.h>

class EpollServer
{
public:
    EpollServer(const char *serverIP, int serverPort);

    ~EpollServer();

    void runLoop();

private:
    bool initServer();
    void setNonBlock(int sock);

    static void listenThread(void *args);

private:
    int             epollFd_;
    int             srvSocket_;
    char            srvIP_[64];
    short           srvPort_;
    pthread_t       listenThread_;
};

#endif  /* ZL_EPOLLSERVER_H */