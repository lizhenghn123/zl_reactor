#include "base/Exception.h"
#include <execinfo.h>
#include <stdlib.h>
#include "base/Demangle.h"
NAMESPACE_ZL_BASE_START

Exception::Exception(const char* errinfo)
    : line_(0),
      filename_("unknown"),
      errmsg_(errinfo)
      
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
    : line_(linenumber),
      filename_(filename),
      errmsg_(errinfo)
{
    trace_stack();
}

Exception::~Exception() throw ()
{
}

void Exception::trace_stack()
{
    static const int len = 256;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len);
    char** strings = ::backtrace_symbols(buffer, nptrs);
    if (strings)
    {
        for (int i = 0; i < nptrs; ++i)
        {
            std::string unmangle;
            if(demangleName(strings[i], unmangle))
            {
                //printf("success   %s\n", unmangle.c_str());
                callStack_.append(unmangle);
            }
            else
            {
                //printf("fail   %s\n", strings[i]);
                callStack_.append(strings[i]);
            }
            callStack_.push_back('\n');
        }
        free(strings);
    }
}

NAMESPACE_ZL_BASE_END
