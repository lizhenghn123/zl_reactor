#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = websocketclient

INCLUDES = -I../../zlreactor

websocketclient_CPPFLAGS = -std=c++0x -O2
websocketclient_LDFLAGS  = -O2 -L../../lib -lzlreactor

websocketclient_SOURCES  = test_websocketclient.cpp
