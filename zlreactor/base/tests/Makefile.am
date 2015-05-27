#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = ZLog_test SmartAssert_test StringUtil_test Daemon_test Exception_Demangle_test ProcessUtil_test Timestamp_test ScopeExitGuard_test

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

Exception_Demangle_test_CPPFLAGS = $(CPPFLAGS)
Exception_Demangle_test_LDFLAGS  = $(CPPFLAGS)
Exception_Demangle_test_SOURCES  = Exception_Demangle_test.cpp

ProcessUtil_test_CPPFLAGS = $(CPPFLAGS)
ProcessUtil_test_LDFLAGS  = $(CPPFLAGS)
ProcessUtil_test_SOURCES  = ProcessUtil_test.cpp

Timestamp_test_LDFLAGS  = $(CPPFLAGS) -lrt
Timestamp_test_SOURCES    = Timestamp_test.cpp

ScopeExitGuard_test_LDFLAGS  = $(CPPFLAGS) -std=c++1y
ScopeExitGuard_test_SOURCES = ScopeExitGuard_test.cpp
