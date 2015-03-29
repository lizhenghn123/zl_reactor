#!/usr/bin/env python 
from socket import *

HOST = 'localhost' 
PORT = 8888 
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR) 
while True:
    data='hello world!'
    #print(data)
    #tcpCliSock.send(data)
    #tcpCliSock.send(data) 
    data = tcpCliSock.recv(BUFSIZ) 
    if not data: 
        break
    print(data)
    tcpCliSock.close()
