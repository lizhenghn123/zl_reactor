#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

int test_max_conn(const char *ip = "127.0.0.1", int port = 5000)
{
    struct sockaddr_in addr;
    int bufsize = 5000;
    socklen_t optlen;
    int connections = 0;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons((short)port);

    while(1)
    {
        int sock;
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            goto sock_err;
        }
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            goto sock_err;
        }

        connections ++;

        if(connections % 1000 == 999)
        {
            //printf("press Enter to continue: ");
            //getchar();
            printf("connections: %d, fd: %d\n", connections, sock);
        }
        usleep(1 * 1000); // 加上此句，连接创建的更快，为什么？？？

        setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    }
sock_err:
    printf("connections: %d\n", connections);
    printf("error: %s\n", strerror(errno));

    return connections;
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        test_max_conn();
    }
    else
    {
        test_max_conn(argv[1], atoi(argv[2]));
    }


    return 0;
}

