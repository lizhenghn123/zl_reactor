#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = echoserver

INCLUDES = -I../../zlreactor

echoserver_CPPFLAGS = -std=c++0x
#echoserver_LDFLAGS = -L/media/sf_D_DRIVE/zl_reactor/lib -libzlreactor.a
echoserver_LDFLAGS = -L../../lib -lzlreactor

echoserver_SOURCES = EchoServer.cpp
