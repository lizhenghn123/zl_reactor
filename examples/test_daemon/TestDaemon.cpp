#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>   // for exit
#include <time.h>
#include <fcntl.h>    // for O_CREAT
#include <string.h>
#include <error.h>
#include "base/Daemonize.h"

int main()
{    
    int ret = zl::base::createDaemonize("/var/run/aout.pid", 1, 1);
    if(ret != 0)
    {
        fprintf(stderr, "create daemon error.\n");
        exit(1);
    }
    printf("create daemon success\n");

    time_t t;
    int fd;
    while(1)
    {
        fd = open("daemon.log", O_WRONLY|O_CREAT|O_APPEND, 0644);   
        if (fd == -1)
        {
            perror("open daemon.log error");
            exit(EXIT_FAILURE);
        }
        t = time(0);
        char *buf = asctime(localtime(&t));
        write(fd, buf, strlen(buf));     //向daemon.log文件中写入当前时间
        printf("time: %s\n", buf);
        close(fd);
        sleep(6);    // 每隔6s写入一次
    }
}