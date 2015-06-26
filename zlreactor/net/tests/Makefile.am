#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Timer_test NetBuffer_test DomainResolver_test HttpRequestParser_test Socket_test\
               Eventfd_test Timerfd_test Signalfd_test \
			   TcpAcceptor_test TcpClient_test EventLoop_test EventLoopThreadPool_test 

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

Timer_test_CPPFLAGS = $(CPPFLAGS)
Timer_test_LDFLAGS  = $(CPPFLAGS)
Timer_test_SOURCES  = Timer_test.cpp

NetBuffer_test_CPPFLAGS = $(CPPFLAGS)
NetBuffer_test_LDFLAGS  = $(CPPFLAGS)
NetBuffer_test_SOURCES  = NetBuffer_test.cpp

DomainResolver_test_CPPFLAGS = $(CPPFLAGS)
DomainResolver_test_LDFLAGS  = $(CPPFLAGS)
DomainResolver_test_SOURCES  = DomainResolver_test.cpp

HttpRequestParser_test_CPPFLAGS = $(CPPFLAGS)
HttpRequestParser_test_LDFLAGS  = $(CPPFLAGS)
HttpRequestParser_test_SOURCES  = HttpRequestParser_test.cpp

Socket_test_SOURCES    = Socket_test.cpp

Eventfd_test_SOURCES   = Eventfd_test.cpp
Timerfd_test_SOURCES   = Timerfd_test.cpp
Signalfd_test_SOURCES  = Signalfd_test.cpp

TcpAcceptor_test_SOURCES = TcpAcceptor_test.cpp

TcpClient_test_SOURCES  = TcpClient_test.cpp

EventLoop_test_SOURCES  = EventLoop_test.cpp

EventLoopThreadPool_test_SOURCES = EventLoopThreadPool_test.cpp
