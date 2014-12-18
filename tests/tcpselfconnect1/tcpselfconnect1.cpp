#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

// http://www.rampa.sk/static/tcpLoopConnect.html
// TCP自连接， 只创建一个连接socket（没有监听socket），向某一固定端口发起连接，
// 循环连接，直接连接成功， 此时可以查看到网络连接上有一个TCP自连接
int main(int argc, char *argv[])
{
  int port = 0;
  int sckfd = 0;
  int opt = 1;
  struct sockaddr_in *remote;
  const char *ip = "127.0.0.1";
  int rc;
  long n = 0;

  if(argc != 2){
    printf("Usage: %s <listen port>\n", argv[0]);
    exit(1);
  }
  port = atoi(argv[1]);

  /* Set to Localhost:Port */
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  assert(inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr))) > 0);
  remote->sin_port = htons(port);

  /* create socket */
  assert((sckfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0);
  assert(setsockopt(sckfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0);

  printf("Trying to connect..."); fflush(stdout);
  while(1) {
    n++;
    rc = connect(sckfd, (struct sockaddr *)remote, sizeof(struct sockaddr)); 
    if(rc < 0){
      if(n % 1000 == 0) { printf("."); fflush(stdout); }
      continue;
    }
    else {
      printf("Connected after %ld tries\n", n);
      break;
    }
  }
  /* Wait for Enter */
  printf("Press Enter to Continue...\n");
  getchar();
  close(sckfd);
  return 0;
}


/***********
TCP state machine allows that TCP client establishes connection using connect() to itself. Look on diagram and focus on Simultaneous Open part.

Preconditions
    Following conditions are needed for the loop:
        1. Connection is established from HOST to HOST on the same IP
        2. Destination TCP Port has to be from range of ephemeral ports configured on OS
        3. Listen port is unbound

Probability
    Preconditions don’t automatically guarantee the loop situation. But they are needed. Loop happens when OS bind the TCP port to client a
ttempting to connect on the same. It can happen only for ephemeral ports.

    The correctly designed TCP client/server application should not use the ephemeral ports for service side listener. Otherwise there is a chance
it will establish unwanted connection to itself - loop.

    In general it depends on various conditions like how OS assign ephemeral ports and how TCP client constructs sockets, but probability increases when:
        TCP client creates for each connection new socket and OS assigns new ephemeral port (done usually in some high-level languages by programmers)

Ephemeral ports
    For the details please read article describing them, basically on linux they can be displayed using:
        $ cat /proc/sys/net/ipv4/ip_local_port_range 
        32768   61000
    Do not use for TCP lister port the value from that range.

example codes such as above.

output
    root@CS235:lizheng# ./a.out 33333
    Trying to connect......Connected after 3853 tries
    Press Enter to Continue...

    root@CS235:bin# lsof -n | grep 33333
    a.out     22623      root    3u     IPv4            5571596        0t0        TCP 127.0.0.1:33333->127.0.0.1:33333 (ESTABLISHED)
    root@CS235:bin# netstat -anp | grep 33333
    tcp        0      0 127.0.0.1:33333             127.0.0.1:33333             ESTABLISHED 22623/./a.out  

another example
	nc -p 33333 127.0.0.1 33333
	
refer: 
    http://www.rampa.sk/static/tcpLoopConnect.html
    http://segmentfault.com/blog/rockybean/1190000002396411

***********/
