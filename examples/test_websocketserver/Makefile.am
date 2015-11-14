#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = websocketserver

INCLUDES = -I../../zlreactor

websocketserver_CPPFLAGS = -std=c++0x -O2
websocketserver_LDFLAGS  = -O2 -L../../lib -lzlreactor

websocketserver_SOURCES  = test_websocketserver.cpp
