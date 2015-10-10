#include "base/Demangle.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")
#elif defined(OS_LINUX)
#include<cxxabi.h>
#else
#error "You must be include OsDefine.h firstly if you want to demangle name"
#endif

NAMESPACE_ZL_BASE_START

bool demangleName(const char *mangled, char *unmangled, size_t buf_size)
{
    // static const size_t max_size = 1024;
#ifdef OS_WINDOWS
    if (::UnDecorateSymbolName(mangled, unmangled, buf_size, UNDNAME_COMPLETE) == 0)
    {

        return false;
    }
    else
    {
        
        return true;
    }
#else
    int status;
    //size_t n = max_size;
    abi::__cxa_demangle(mangled, unmangled, &buf_size, &status);
    if(status == 0)
    {
        //printf("Name after  Mangled : %s ; Name before Mangled : %s\n", unmangled, mangled);
        return true;
    }
    //printf("Name after  Mangled fail: %s Name before Mangled : %s\n", unmangled, mangled);
    return false;
#endif
}

bool demangleName(const char *mangled, std::string& unmangled)
{
    static const size_t max_size = 1024;
    char result[max_size];
    if(demangleName(mangled, result, max_size))
    {
        unmangled = result;
        return true;
    }
    return false;
}

NAMESPACE_ZL_BASE_END