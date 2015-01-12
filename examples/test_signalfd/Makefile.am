#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = test_signalfd

INCLUDES = -I../../zlreactor

test_signalfd_CPPFLAGS = -std=c++0x
test_signalfd_LDFLAGS = -L../../lib -lzlreactor

test_signalfd_SOURCES = TestSignalFd.cpp
