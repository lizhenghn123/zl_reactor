#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Logger_test SmartAssert_test StringUtil_test Daemon_test Exception_Demangle_test ProcessUtil_test Timestamp_test ScopeExitGuard_test \
			   StlUtil_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x -O2
LDFLAGS  = -O2 -L../../../lib -lzlreactor

Logger_test_SOURCES  = Logger_test.cpp

SmartAssert_test_SOURCES  = SmartAssert_test.cpp

StringUtil_test_SOURCES  = StringUtil_test.cpp

Daemon_test_SOURCES  = Daemon_test.cpp

Exception_Demangle_test_SOURCES  = Exception_Demangle_test.cpp

ProcessUtil_test_CPPFLAGS = $(CPPFLAGS)
ProcessUtil_test_LDFLAGS  = $(CPPFLAGS)
ProcessUtil_test_SOURCES  = ProcessUtil_test.cpp

Timestamp_test_LDFLAGS  = $(CPPFLAGS) -lrt
Timestamp_test_SOURCES    = Timestamp_test.cpp

ScopeExitGuard_test_LDFLAGS  = $(CPPFLAGS) -std=c++1y
ScopeExitGuard_test_SOURCES  = ScopeExitGuard_test.cpp
 
StlUtil_test_SOURCES         = StlUtil_test.cpp
