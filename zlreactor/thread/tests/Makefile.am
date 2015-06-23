#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Atomic_test BlockingQueue_test CountDownLatch_test Thread_test ThreadLocal_test Mutex_test ThreadGroup_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

#Atomic_test_CPPFLAGS        = $(CPPFLAGS)
#Atomic_test_LDFLAGS         = $(CPPFLAGS)
Atomic_test_SOURCES          = Atomic_test.cpp

Mutex_test_SOURCES           = Mutex_test.cpp

BlockingQueue_test_SOURCES   = BlockingQueue_test.cpp

CountDownLatch_test_SOURCES  = CountDownLatch_test.cpp

#Thread_test_CPPFLAGS        = $(CPPFLAGS)
#Thread_test_LDFLAGS         = $(CPPFLAGS)
Thread_test_SOURCES          = Thread_test.cpp

ThreadLocal_test_SOURCES     = ThreadLocal_test.cpp

ThreadGroup_test_SOURCES     = ThreadGroup_test.cpp
