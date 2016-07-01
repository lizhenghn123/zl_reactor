#include <iostream>
#include <assert.h>
#include <string>
#include "zlreactor/net/InetAddress.h"
#include "zlreactor/net/Socket.h"
#include "zlreactor/net/SocketUtil.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void test_InetAddress()
{
    {
        InetAddress addr(8888);
        assert(addr.port()== 8888);
        assert(addr.ip() == string("0.0.0.0"));
        assert(addr.ipPort() == string("0.0.0.0:8888"));
    }
    {
        InetAddress addr("192.168.14.7", 8888);
        assert(addr.ip() == string("192.168.14.7"));
        assert(addr.ipPort() == string("192.168.14.7:8888"));
    }
    {
        InetAddress addr("192.168.14.7", 8888);
        Socket socket(SocketUtil::createSocket());
        assert(socket.fd() > 0);
        bool ret = socket.bind(addr);
        assert(ret);
        ret = socket.listen(1024);
        assert(ret);
        cout << "server listening on " << addr.ipPort() << ", waiting for incoming....\n";
        while(true)
        {
            InetAddress clientAddr;
            ZL_SOCKET clientFd = socket.accept(&clientAddr);    // test: nc 192.168.14.7 8888 
            cout << "accept one client from " << clientAddr.ipPort() << "\n";
            SocketUtil::closeSocket(clientFd);
        }
    }
}

int main()
{
	test_InetAddress();
    printf("###### GAME OVER ######");
}

