#include <iostream>
#include "net/EventLoop.h"
#include "net/http/HttpServer.h"
using namespace std;
using namespace zl::net;


int main()
{
    EventLoop loop;
    HttpServer server(&loop, InetAddress("127.0.0.1", 8888), "myHttpServer");
    server.setThreadNum(2);
    server.start();
    loop.loop();

    return 0;
}
