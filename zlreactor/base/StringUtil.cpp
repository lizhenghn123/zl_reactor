#include "zlreactor/base/StringUtil.h"
#include "zlreactor/Define.h"
#include <stdarg.h>

namespace zl
{
namespace base
{

#ifdef OS_WINDOWS
#define VADUP(aq, ap)    (backup_ap = ap)
#else
#define VADUP(aq, ap)    va_copy(backup_ap, ap)
#endif

size_t stringFormatAppendImpl(std::string *dst, const char *format, va_list ap)
{
    char space[1024];    // just try with a small fixed size buffer

    va_list backup_ap;       // see "man va_start"
    VADUP(backup_ap, ap);

    int result = vsnprintf(space, sizeof(space), format, backup_ap);
    va_end(backup_ap);
    if ((result >= 0) && (result < static_cast<int>(sizeof(space))))
    {
        dst->append(space, result);
        return result;
    }
    
    int length = sizeof(space);
    while (true)        // Repeatedly increase buffer size until it fits
    {
        if (result < 0)
        {
            length *= 2;
        }
        else
        {
            length = result + 1;
        }

        char* buf = new char[length];
        VADUP(backup_ap, ap);    // Restore the va_list before we use it again
        result = vsnprintf(buf, length, format, backup_ap);
        va_end(backup_ap);
        if ((result >= 0) && (result < length))
        {
            dst->append(buf, result);
            delete[] buf;
            break;
        }
        delete[] buf;
    }

    return result;
}

size_t stringFormatAppend(std::string *dst, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    size_t result = stringFormatAppendImpl(dst, format, ap);
    va_end(ap);
    return result;
}

size_t stringFormat(std::string *dst, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    dst->clear();
    size_t result = stringFormatAppendImpl(dst, format, ap);
    va_end(ap);
    return result;
}

std::string stringFormat(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    std::string result;
    stringFormatAppendImpl(&result, format, ap);
    va_end(ap);
    return result;
}

} // namespace base
} // namespace zl
