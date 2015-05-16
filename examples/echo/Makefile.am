#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = echo_test

INCLUDES = -I../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../lib -lzlreactor

#echoserver_test_CPPFLAGS = $(CPPFLAGS)
#echoserver_test_LDFLAGS  = $(CPPFLAGS)
#echoserver_test_SOURCES  = main.cpp EchoServer.cpp EchoClient.cpp
#echoclient_test_SOURCES  = main.cpp EchoServer.cpp EchoClient.cpp

echo_test_SOURCES  = main.cpp EchoServer.cpp EchoClient.cpp