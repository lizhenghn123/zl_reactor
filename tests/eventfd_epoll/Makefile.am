#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = eventfd_epoll

INCLUDES = -I../../zlreactor

eventfd_epoll_CPPFLAGS = -std=c++0x
eventfd_epoll_LDFLAGS = -L../../lib -lzlreactor

eventfd_epoll_SOURCES = eventfd_epoll.cpp
