#include <stdio.h>
#include <stdlib.h>
#include "EchoServer.h"

int main(int argc, char *argv[])
{
    EchoServer *server;

    if(argc != 3)
    {
        printf("usage : [%s] serverip serverport; example: [%s] 127.0.0.1 5000\n", argv[0], argv[0]);
        printf("now use default settings : 127.0.0.1:8888\n");
        server = new EchoServer("127.0.0.1", 8888);
    }
    else
    {
        server = new EchoServer(argv[1], atoi(argv[2]));
    }

    server->runLoop();

	return 0;
}
