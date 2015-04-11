#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Atomic_test BlockingQueue_test CountDownLatch_test Thread_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

Atomic_test_CPPFLAGS = $(CPPFLAGS)
Atomic_test_LDFLAGS  = $(CPPFLAGS)
Atomic_test_SOURCES  = Atomic_test.cpp

BlockingQueue_test_CPPFLAGS = $(CPPFLAGS)
BlockingQueue_test_LDFLAGS  = $(CPPFLAGS)
BlockingQueue_test_SOURCES  = BlockingQueue_test.cpp

CountDownLatch_test_CPPFLAGS = $(CPPFLAGS)
CountDownLatch_test_LDFLAGS  = $(CPPFLAGS)
CountDownLatch_test_SOURCES  = CountDownLatch_test.cpp

Thread_test_CPPFLAGS = $(CPPFLAGS)
Thread_test_LDFLAGS  = $(CPPFLAGS)
Thread_test_SOURCES  = Thread_test.cpp

