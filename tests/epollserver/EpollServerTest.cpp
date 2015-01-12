#include <stdio.h>
#include <stdlib.h>
#include "EpollServer.h"

int main(int argc, char *argv[])
{
    EpollServer *server;

    if(argc != 3)
    {
        printf("usage : [%s] serverip serverport max_client_sockets; example: [%s] 127.0.0.1 5000\n", argv[0], argv[0]);
        printf("now use default settings : 127.0.0.1:8888\n");
        server = new EpollServer("127.0.0.1", 8888);
    }
    else
    {
        server = new EpollServer(argv[1], atoi(argv[2]));
    }

    server->runLoop();

	return 0;
}
