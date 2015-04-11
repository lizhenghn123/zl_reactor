#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = NetBuffer_test DomainResolver_test EventLoop_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

DomainResolver_test_CPPFLAGS = $(CPPFLAGS)
DomainResolver_test_LDFLAGS  = $(CPPFLAGS)
DomainResolver_test_SOURCES  = DomainResolver_test.cpp

EventLoop_test_CPPFLAGS = $(CPPFLAGS)
EventLoop_test_LDFLAGS  = $(CPPFLAGS)
EventLoop_test_SOURCES  = EventLoop_test.cpp

NetBuffer_test_CPPFLAGS = $(CPPFLAGS)
NetBuffer_test_LDFLAGS  = $(CPPFLAGS)
NetBuffer_test_SOURCES  = NetBuffer_test.cpp
