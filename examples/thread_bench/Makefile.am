#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = thread_bench

INCLUDES = -I../../zlreactor

thread_bench_CPPFLAGS = -std=c++0x
thread_bench_LDFLAGS = -L../../lib -lzlreactor

thread_bench_SOURCES = thread_bench.cpp
