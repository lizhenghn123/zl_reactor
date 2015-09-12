#CXXFLAGS += -Wall -static -g -O2 -fPIC

bin_PROGRAMS = Logger_test SmartAssert_test StringUtil_test Daemon_test Demangle_test ProcessUtil_test Timestamp_test ScopeExitGuard_test \
			   StlUtil_test AsyncLogger_test Exception_test MasterWorkerProcess_test ConfigFile_test StringPiece_test ObjectPool_test

INCLUDES = -I../../../zlreactor
CPPFLAGS = -std=c++0x -O2
LDFLAGS  = -O2 -L../../../lib -lzlreactor -lrt

Logger_test_SOURCES          = Logger_test.cpp

AsyncLogger_test_SOURCES     = AsyncLogger_test.cpp

SmartAssert_test_SOURCES     = SmartAssert_test.cpp

StringUtil_test_SOURCES      = StringUtil_test.cpp

Daemon_test_SOURCES          = Daemon_test.cpp

Demangle_test_SOURCES        = Demangle_test.cpp

Exception_test_LDFLAGS       = -rdynamic
Exception_test_SOURCES       = Exception_test.cpp

ProcessUtil_test_CPPFLAGS    = $(CPPFLAGS)
ProcessUtil_test_LDFLAGS     = $(CPPFLAGS)
ProcessUtil_test_SOURCES     = ProcessUtil_test.cpp

MasterWorkerProcess_test_SOURCES = MasterWorkerProcess_test.cpp

Timestamp_test_LDFLAGS       = $(CPPFLAGS) -lrt
Timestamp_test_SOURCES       = Timestamp_test.cpp

ScopeExitGuard_test_LDFLAGS  = $(CPPFLAGS) -std=c++1y
ScopeExitGuard_test_SOURCES  = ScopeExitGuard_test.cpp
 
StlUtil_test_SOURCES         = StlUtil_test.cpp

ConfigFile_test_SOURCES      = ConfigFile_test.cpp

StringPiece_test_SOURCES     = StringPiece_test.cpp

ObjectPool_test_SOURCES      = ObjectPool_test.cpp
