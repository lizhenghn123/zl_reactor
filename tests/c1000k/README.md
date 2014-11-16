https://github.com/ideawu/c1000k
This is the TCP server-client suit to help you test if your OS supports c1000k(1 million connections).

## Usage

### Download and compile

    wget --no-check-certificate https://github.com/ideawu/c1000k/archive/master.zip
    unzip master.zip
    cd c1000k-master
    make

### start the server, listen on port 7000~7099

    ./server 7000

### run the client

    ./client 127.0.0.1 7000

The server will ouput message like this and quit:

    connections: 153
    error: Too many open files

It says the server can only accept 153 connections, it reaches the max-open-files limitation so it quit.

The client will output message like this:

    connections: 154
    error: Connection refused


######## http://idning.github.io/icomet-cr.html
ideawu 提供了一个 c1000k 代码:  https://github.com/ideawu/c1000k

要测试c1000k 主要两个问题:

是server 端, 每个sokcet 大约需要占用0.5M内存(内核sokcet缓冲区内存), 所以总共需要500G内存.
client端, client 只能开大约3w-5w个client端口(对每个server ip+port, 只能有3w-5w连接)
ideawu是这样解决的: 1. server 端每accept一个连接, 都设置它的缓冲区大小为5k:

     bufsize = 5000;
     setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
     setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));

这样1000k连接只需要10G内存.
server 端开100个port, 这样每个客户端3w端口, 就可以开300w个连接.

这时候, 限制就只有fd个数限制了.

我启动的时候设置好limit就可以

$ ulimit -n 1024000 
$ ./server 6000
$ ulimit -n 1024000 
$ ./client 127.0.0.1 6000

可以一直跑到:
connections: 1023897
error: Too many open files
