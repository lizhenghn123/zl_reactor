#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Atomic_test BlockingQueue_test CountDownLatch_test CyclicBarrier_test Thread_test ThreadLocal_test \
               Mutex_test ThreadGroup_test BlockingQueueWorker_test ThreadId_test BlockingQueue_bench \
			   ConcurrentQueue_test ConcurrentQueue_bench ConcurrentHashMap_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

#Atomic_test_CPPFLAGS        = $(CPPFLAGS)
#Atomic_test_LDFLAGS         = $(CPPFLAGS)
Atomic_test_SOURCES          = Atomic_test.cpp

Mutex_test_CPPFLAGS          = $(CPPFLAGS) -O2
Mutex_test_SOURCES           = Mutex_test.cpp

BlockingQueue_test_SOURCES   = BlockingQueue_test.cpp

BlockingQueue_bench_SOURCES  = BlockingQueue_bench.cpp

CountDownLatch_test_SOURCES  = CountDownLatch_test.cpp

CyclicBarrier_test_SOURCES   = CyclicBarrier_test.cpp

#Thread_test_CPPFLAGS        = $(CPPFLAGS)
#Thread_test_LDFLAGS         = $(CPPFLAGS)
Thread_test_SOURCES          = Thread_test.cpp

ThreadId_test_SOURCES        = ThreadId_test.cpp

ThreadLocal_test_SOURCES     = ThreadLocal_test.cpp

ThreadGroup_test_SOURCES     = ThreadGroup_test.cpp

BlockingQueueWorker_test_SOURCES = BlockingQueueWorker_test.cpp

ConcurrentQueue_test_SOURCES     = ConcurrentQueue_test.cpp

ConcurrentQueue_bench_LDFLAGS    = $(CPPFLAGS) -O2 -DNDEBUG
ConcurrentQueue_bench_SOURCES    = ConcurrentQueue_bench.cpp

ConcurrentHashMap_test_SOURCES   = ConcurrentHashMap_test.cpp
