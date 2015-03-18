#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = httpserver

INCLUDES = -I../../zlreactor

httpserver_CPPFLAGS = -std=c++0x
httpserver_LDFLAGS = -L../../lib -lzlreactor

httpserver_SOURCES = test_httpserver.cpp
