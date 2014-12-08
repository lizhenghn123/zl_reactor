#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "EpollClient.h"

EpollClient *pollclient = NULL;

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("usage : [%s] serverip serverport max_client_sockets; example: [%s] 127.0.0.1 5000 1000 \n", argv[0], argv[0]);
        return 0;
    }

	pollclient = new EpollClient(argv[1], atoi(argv[2]), atoi(argv[3]));
	pollclient->Start();

	pollclient->RunLoop();
	
    return 0;
}
