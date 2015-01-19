#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = test_tcpclient

INCLUDES = -I../../zlreactor

test_tcpclient_CPPFLAGS = -std=c++0x
test_tcpclient_LDFLAGS = -L../../lib -lzlreactor

test_tcpclient_SOURCES = test_tcpclient.cpp
