#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = httpserver

INCLUDES = -I../../zlreactor

httpserver_CPPFLAGS = -std=c++0x -O2
httpserver_LDFLAGS = -O2 -L../../lib -lzlreactor

httpserver_SOURCES = test_httpserver.cpp
