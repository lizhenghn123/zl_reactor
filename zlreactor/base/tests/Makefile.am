#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = ZLog_test SmartAssert_test StringUtil_test Daemon_test  

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x
LDFLAGS  = -L../../../lib -lzlreactor

ZLog_test_CPPFLAGS = $(CPPFLAGS)
ZLog_test_LDFLAGS  = $(CPPFLAGS)
ZLog_test_SOURCES  = ZLog_test.cpp

SmartAssert_test_CPPFLAGS = $(CPPFLAGS)
SmartAssert_test_LDFLAGS  = $(CPPFLAGS)
SmartAssert_test_SOURCES  = SmartAssert_test.cpp

StringUtil_test_CPPFLAGS = $(CPPFLAGS)
StringUtil_test_LDFLAGS  = $(CPPFLAGS)
StringUtil_test_SOURCES  = StringUtil_test.cpp

Daemon_test_CPPFLAGS = $(CPPFLAGS)
Daemon_test_LDFLAGS  = $(CPPFLAGS)
Daemon_test_SOURCES  = Daemon_test.cpp