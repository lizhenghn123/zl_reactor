#include "base/Exception.h"
#include <stdlib.h>
#ifdef OS_WINDOWS
#else
#include <execinfo.h>
#define DO_NAME_DEMANGLE
#endif
#ifdef DO_NAME_DEMANGLE
#include "base/Demangle.h"
#endif
NAMESPACE_ZL_BASE_START

Exception::Exception(const char* errinfo)
    : line_(0)
    , filename_("unknown filename")
    , errmsg_(errinfo)
{
    trace_stack();
}

Exception::Exception(const char *filename, int linenumber, const char* errinfo)
    : line_(linenumber),
      filename_(filename),
      errmsg_(errinfo)
      
{
    trace_stack();
}

Exception::Exception(const char *filename, int linenumber, const std::string& errinfo)
    : line_(linenumber)
    , filename_(filename)
    , errmsg_(errinfo)
{
    trace_stack();
}

Exception::~Exception() throw ()
{
}

void Exception::trace_stack()
{
#ifdef OS_WINDOWS
#else
    static const int len = 256;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len);
    char** strings = ::backtrace_symbols(buffer, nptrs);
    if (!strings)
        return;

    for (int i = 0; i < nptrs; ++i)
    {
    #ifndef DO_NAME_DEMANGLE
        callStack_.append(strings[i]);
    #else
        std::string line(strings[i]);  // ./test(_ZN6detail12c_do_nothingEfi+0x44) [0x401974]
        size_t pos1 = line.find("(");
        size_t pos2 = line.find("+");
        if (pos1 > 0 && pos2 > 0)
            line = line.substr(pos1, pos2);

        std::string unmangle;
        if (demangleName(line.c_str(), unmangle))
        {
            callStack_.append(unmangle);
        }
        else
        {
            callStack_.append(strings[i]);
        }
    #endif
        callStack_.push_back('\n');
    }
    free(strings);
#endif
}

NAMESPACE_ZL_BASE_END
