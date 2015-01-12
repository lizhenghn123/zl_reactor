#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = test_daemon

INCLUDES = -I../../zlreactor

test_daemon_CPPFLAGS = -std=c++0x -Wall
test_daemon_LDFLAGS = -L../../lib -lzlreactor

test_daemon_SOURCES = TestDaemon.cpp
