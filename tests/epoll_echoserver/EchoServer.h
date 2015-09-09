#ifndef ZL_ECHOSERVER_H
#define ZL_ECHOSERVER_H
#include <pthread.h>
class EpollPoller;

class EchoServer
{
public:
    EchoServer(const char *serverIP, int serverPort);

    ~EchoServer();

    void runLoop();

private:
    bool initServer();
    void setNonBlock(int sock);

    static void listenThread(void *args);

private:
    int             srvSocket_;
    char            srvIP_[64];
    short           srvPort_;
    pthread_t       listenThread_;
    EpollPoller*    epoller_;
};

#endif  /* ZL_ECHOSERVER_H */