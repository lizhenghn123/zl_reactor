#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = pingpong_server pingpong_client

INCLUDES = -I../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS = -L../../lib -lzlreactor

pingpong_server_SOURCES = pingpong_server.cpp

pingpong_client_SOURCES = pingpong_client.cpp