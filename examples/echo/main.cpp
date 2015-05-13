/*************************************************************************
	File Name   : main.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月12日 星期二 19时57分02秒
 ************************************************************************/
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/NetBuffer.h"
#include "net/TcpConnection.h"
#include "net/TcpClient.h"
#include "base/ZLog.h"
#include "EchoServer.h"
#include "EchoClient.h"

using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;

void runServerLoop(int port, int reactor_threads)
{
    EventLoop loop;
    InetAddress listenAddr(port);

    EchoServer server(&loop, listenAddr, reactor_threads);

    printf("server listening on %s.....\n", listenAddr.ipPort().c_str());

    server.start();
    loop.loop();
}

vector<EchoClient*> g_clients;
void runClientLoop(const char *ip, int port, int clientNum)
{
    assert(clientNum > 0);

    EventLoop loop;
    InetAddress serverAddr(ip, port);

    for (int i = 1; i <= clientNum; ++i)
    {
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "EchoClient%d", i);
		cout << buf << "\n";
        EchoClient *client = new EchoClient(&loop, serverAddr, buf);
        g_clients.push_back(client);
    }

	srand((unsigned)time(NULL));
    for (int i = 0; i < g_clients.size(); ++i)
    {
        g_clients[i]->connect();
		int seconds = rand() % 10;
		//LOG_INFO("socket [%d] would be stop after [%d] seconds", g_clients[i]->fd(), seconds);
		loop.addTimer(std::bind(&EchoClient::stop, g_clients[i]), seconds);
    }

    loop.loop();

}

namespace test_client
{

    TcpConnectionPtr clientConnection;

    void clientConnectionCallback(const TcpConnectionPtr& conn)
    {
        cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
            << " is " << (conn->connected() ? "UP" : "DOWN") << "\n";
        if (conn->connected())
        {
            clientConnection = conn;
            conn->setNoDelay(true);
        }
        else
        {
            clientConnection.reset();
        }
    }

    void clientMessageCallback(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp receiveTime)
    {
        string msg(buf->retrieveAllAsString());
        //cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
        //    << msg.data() << "], received at " << time.toString() << "\n";
        cout << "client recv data : " << msg << " at " <<receiveTime.toString() << "\n";
        //conn->send(msg.data(), msg.size());
    }

    void sendDataByClient()
    {
        static int64_t i = 0;
        if (clientConnection)
        {
            Timestamp now = Timestamp::now();
            char str[] = "hello world";
            cout << "client send data : " << str << " at " << now.toString() <<"\n";
            clientConnection->send(str, sizeof(str));
        }
    }
	
	    class TestTcpClient
    {
    public:
        TestTcpClient(EventLoop* loop, const InetAddress& listenAddr)
            : loop_(loop), client_(loop, listenAddr, "TestTcpClient")
        {
            client_.setConnectionCallback(std::bind(&TestTcpClient::onConnection, this, std::placeholders::_1));
            client_.setMessageCallback(std::bind(&TestTcpClient::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
            // 标准输入缓冲区中有数据的时候，回调TestTcpClient::handleRead
            stdinChannel_ = new Channel(loop, 0);
            stdinChannel_->setReadCallback(std::bind(&TestTcpClient::handleRead, this));
            stdinChannel_->enableReading();		// 关注可读事件
        }
        ~TestTcpClient()
        {
            delete stdinChannel_;
        }
        void connect()
        {
            client_.connect();
        }
    private:
        void onConnection(const TcpConnectionPtr& conn)
        {
            cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
                << " is " << (conn->connected() ? "UP" : "DOWN") << "\n";
            clientConnection = conn;
            if (conn->connected())
            {
                //conn->setNoDelay(true);
            }
            else
            {
                //clientConnection.reset();
            }
        }
        void onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, Timestamp time)
        {
            string msg(buf->retrieveAllAsString());
            printf("onMessage(): recv a message [%s]\n", msg.c_str());
            //LOG_INFO("[%d] recv %d bytes at %s", conn->fd(), msg.size(), time.toString().c_str());
        }
        // 标准输入缓冲区中有数据的时候，回调该函数
        void handleRead()
        {
            char buf[1024] = { 0 };
            fgets(buf, 1024, stdin);
            buf[strlen(buf) - 1] = '\0';		// 去除\n
            clientConnection->send(buf);
        }
    private:
        EventLoop *loop_;
        TcpClient client_;
        Channel *stdinChannel_;		// 标准输入Channel
        TcpConnectionPtr clientConnection;
    }; 
}
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("*** usage ***\n");
        printf("run echoserver: %s -s <listen port> <reactors num>\n", argv[0]);
        printf("run echoclient: %s <server ip> <server port> <clients num>\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        runServerLoop(atoi(argv[2]), atoi(argv[3]));
    }
    else
    {
		EventLoop loop;
		InetAddress serverAddr("127.0.0.1", 8888); 
		
        cout << argv[1] << "\t" << atoi(argv[2]) << "\t" << atoi(argv[3]) << "\n";
        runClientLoop(argv[1], atoi(argv[2]), atoi(argv[3]));
		
		EchoClient client0(&loop, serverAddr);
        client0.connect();
        loop.loop();
		
		test_client::TestTcpClient client1(&loop, serverAddr);
        client1.connect();
        loop.loop();
		
		
		
		
		TcpClient client(&loop, serverAddr, "fff");
        client.enableRetry();
        client.setConnectionCallback(test_client::clientConnectionCallback);
        client.setMessageCallback(test_client::clientMessageCallback);
        client.connect();
        loop.addTimer(test_client::sendDataByClient, 1, true);   //启动一个每1s运行一次的定时器
        loop.loop();

    }

    return 0;
}

