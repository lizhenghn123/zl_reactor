#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = test_countdownlatch

INCLUDES = -I../../zlreactor

test_countdownlatch_CPPFLAGS = -std=c++0x
test_countdownlatch_LDFLAGS = -L../../lib -lzlreactor

test_countdownlatch_SOURCES = test_CountDownLatch.cpp
