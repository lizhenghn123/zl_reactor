#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/ByteBuffer.h"
#include "zlreactor/net/EventLoopThreadPool.h"
#include "zlreactor/net/TcpClient.h"
#include "zlreactor/base/Logger.h"
#include "zlreactor/thread/Atomic.h"
using namespace std;
using namespace zl::net;
using namespace zl::thread;

class Client;

class Session : zl::NonCopy
{
public:
    Session(EventLoop *loop,
        const InetAddress& serverAddr,
        const string& name,
        Client *owner)
        : client_(loop, serverAddr, name),
        owner_(owner),
        bytesRead_(0),
        bytesWritten_(0),
        messagesRead_(0)
    {
        client_.setConnectionCallback(
            std::bind(&Session::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(
            std::bind(&Session::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void start()
    {
        client_.connect();
    }

    void stop()
    {
        client_.disconnect();
    }

    int64_t bytesRead() const
    {
        return bytesRead_;
    }

    int64_t messagesRead() const
    {
        return messagesRead_;
    }

private:

    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp)
    {
        ++messagesRead_;
        bytesRead_ += buf->readableBytes();
        bytesWritten_ += buf->readableBytes();
        //printf("read : %s\n", buf->toString().c_str());
        conn->send(buf);
    }

    TcpClient client_;
    Client *owner_;
    int64_t bytesRead_;
    int64_t bytesWritten_;
    int64_t messagesRead_;
};

class Client : zl::NonCopy
{
public:
    Client(EventLoop *loop, const InetAddress& serverAddr,
        int blockSize, int sessionCount, int timeout, int threadCount)
        : loop_(loop),
        threadPool_(loop),
        sessionCount_(sessionCount),
        timeout_(timeout)
    {
        loop->addTimer(std::bind(&Client::handleTimeout, this), timeout);
        if(threadCount > 1)
        {
            threadPool_.setMultiReactorThreads(threadCount);
        }
        threadPool_.start();

        for(int i = 0; i < blockSize; ++i)
        {
            message_.push_back(static_cast<char>(i % 128));
        }

        for(int i = 0; i < sessionCount; ++i)
        {
            char buf[32];
            snprintf(buf, sizeof buf, "C%05d", i);
            Session *session = new Session(threadPool_.getNextLoop(), serverAddr, buf, this);
            session->start();
            sessions_.push_back(session);
        }
    }

    const string& message() const
    {
        return message_;
    }

    void onConnect()
    {
        if(numConnected_.increment() == sessionCount_)
        {
            printf("all connected\n");
        }
    }

    void onDisconnect(const TcpConnectionPtr& conn)
    {
        if(numConnected_.decrement() == 0)
        {
            printf("all disconnected\n");
            int64_t totalBytesRead = 0;
            int64_t totalMessagesRead = 0;
            for(std::vector<Session*>::iterator it = sessions_.begin(); it != sessions_.end(); ++it)
            {
                totalBytesRead += (*it)->bytesRead();
                totalMessagesRead += (*it)->messagesRead();
            }
            printf("%ld total bytes read\n", totalBytesRead);
            printf("%ld total messages read\n", totalMessagesRead);
            printf("%f average message size\n", static_cast<double>(totalBytesRead) / static_cast<double>(totalMessagesRead));
            printf("%f QPS\n", static_cast<double>(totalMessagesRead) / timeout_);
            printf("%f MiB/s throughput\n", static_cast<double>(totalBytesRead) / (timeout_ * 1024 * 1024));
            conn->getLoop()->queueInLoop(std::bind(&Client::quit, this));
        }
    }

private:
    void quit()
    {
        loop_->queueInLoop(std::bind(&EventLoop::quit, loop_));
    }

    void handleTimeout()
    {
        printf("handleTimeout\n");
        std::for_each(sessions_.begin(), sessions_.end(), std::mem_fn(&Session::stop));
    }

    EventLoop *loop_;
    EventLoopThreadPool threadPool_;
    int sessionCount_;
    int timeout_;
    std::vector<Session*> sessions_;
    string message_;
    AtomicInt32 numConnected_;
};

void Session::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setNoDelay(true);
        conn->send(owner_->message());
        owner_->onConnect();
    }
    else
    {
        owner_->onDisconnect(conn);
    }
}

int main(int argc, char *argv[])
{
    if(argc != 7)
    {
        fprintf(stderr, "Usage: %s <host_ip> <port> <threads> <blocksize> <clients> <time>\n", argv[0]);
        fprintf(stderr, "for example : %s 0.0.0.0 8888 10 100 1000 5. It means that:\n", argv[0]);
        fprintf(stderr, "\tserver address is 0.0.0.0:8888, and\n");
        fprintf(stderr, "\tcreate 10 threads, message size is 100 Byte, create 1000 clients, and run 5 seconds\n");
    }
    else
    {
        LOG_SET_PRIORITY(zl::base::ZL_LOG_PRIO_WARNING);

        const char *ip = argv[1];
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        int threadCount = atoi(argv[3]);
        int blockSize = atoi(argv[4]);
        int sessionCount = atoi(argv[5]);
        int timeout = atoi(argv[6]);

        EventLoop loop;
        InetAddress serverAddr(ip, port);

        Client client(&loop, serverAddr, blockSize, sessionCount, timeout, threadCount);
        loop.loop();
    }
}
