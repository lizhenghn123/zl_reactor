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

// 使用同一端口同时充当TCP client和Server， 需要注意：
// 在client端通过bind使用本机指定端口比如8888与远端服务器通信，不指定的话会随机分配
// 然后在server端使用相同端口创建监听socket，等待其他客户端的连接

#define PORT 8888
#define HOST "127.0.0.1"

//线程提供TCP服务器服务，绑定2012端口
void *serve_proc(void *arg)
{
    printf("serve_proc start \n");
    int port = *((int *)(arg));
    printf("server start listening on port %d\n", port);
    int sock_fd;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket error\n");
        exit(1);
    }

    int reuseport = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseport, sizeof(int));

    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(HOST);
    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind error");
        exit(-1);
    }
    if(listen(sock_fd, 5) < 0)
    {
        perror("listen error");
        exit(-1);
    }

    char recvbuf[100], sendbuf[100];
    int client_size = sizeof(struct sockaddr_in);
	int ret = 0;
    while(true)
    {
        struct sockaddr_in client;
        int conn_fd = accept(sock_fd, (struct sockaddr *)&client, (socklen_t *)&client_size);
        printf("cannot go this, because the client tcp self-connect\n");
        if(conn_fd < 0)
        {
            perror("accept error");
            exit(-1);
        }
        while(1)
        {
            ret = recv(conn_fd, recvbuf, sizeof(recvbuf), 0);
            if(ret < 0)
            {
                printf("server recv data from [%d] fail\n", conn_fd);
                close(conn_fd);
                exit(-1);
            }
            recvbuf[ret] = '\0';
            if(strncmp(recvbuf, "quit", 4) == 0)
            {
                break;
            }
            printf("server recv data from [%d] success : (%s, %d)\n", conn_fd, recvbuf, ret);
            sprintf(sendbuf, "server got %d bytes\n", ret);
            printf("---------%s----------\n", sendbuf);
            //ret = send(conn_fd, sendbuf, strlen(sendbuf), 0);
            ret = send(conn_fd, "ok", 2, 0);
            if(ret < 0)
            {
                close(conn_fd);
                exit(-1);
            }
        }
        close(conn_fd);
    }
}

int main(int argc, char *argv[])
{
    int sock_fd;
    struct sockaddr_in server, client;
    int flag = 0;
    int reuseport = 1;
    int local_port = PORT;

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket error\n");
        exit(1);
    }
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseport, sizeof(int));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(HOST);

    //使用本地指定端口建立TCP连接
    struct hostent *he;
    he = gethostbyname(HOST);
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(local_port);
    client.sin_addr = *((struct in_addr *)he->h_addr);;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseport, sizeof(int));
    if(bind(sock_fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind error");
        exit(-1);
    }

    struct sockaddr_in local;
    socklen_t local_len = sizeof(local);
    //获取TCP连接的本地IP端口信息
    getsockname(sock_fd, (struct sockaddr *)&local, &local_len);
    char local_ip[100];
    inet_ntop(AF_INET, &local.sin_addr, local_ip, sizeof(local_ip));
    printf("local host %s and port %d\n", local_ip, ntohs(local.sin_port));
    local_port = ntohs(local.sin_port);

    //server：绑定端口提供TCP服务
    pthread_t id;
    if(pthread_create(&id, NULL, serve_proc, (void *)&local_port) != 0)
    {
        perror("create thread error");
        exit(-1);
    }
    sleep(2);   // wait for server socket create


    if(connect(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }

    //client：使用同样端口充当客户端访问远程服务器
    char sendbuf[100], recvbuf[100];
    int ret = 0;
    while(true)
    {
        printf("input msg: ");
        scanf("%s", sendbuf);
        ret = send(sock_fd, sendbuf, strlen(sendbuf), 0);
        if(ret <= 0)
        {
            perror("send error");
            break;
        }
        else
        {
            printf("client [%d] send data success : (%s, %d)\n", sock_fd, sendbuf, ret);
        }
        char recvbuf[100];
        memset(recvbuf, 0 , 100);
        ret = recv(sock_fd, recvbuf, 100, 0);
        printf("client [%d] recv data success : (%s, %d)\n", sock_fd, recvbuf, ret);
    }
}
