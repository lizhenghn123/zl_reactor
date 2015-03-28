#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = test_eventloop

INCLUDES = -I../../zlreactor

test_eventloop_CPPFLAGS = -std=c++0x
test_eventloop_LDFLAGS = -L../../lib -lzlreactor

test_eventloop_SOURCES = test_eventloop.cpp
