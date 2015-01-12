#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = timerqueue

INCLUDES = -I../../zlreactor

timerqueue_CPPFLAGS = -std=c++0x
timerqueue_LDFLAGS = -L../../lib -lzlreactor

timerqueue_SOURCES = Timer_Test.cpp \
					 TimerQueue.cpp \
					 Timer.cpp
