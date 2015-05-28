#ifndef ZL_UUID_H
#define ZL_UUID_H
#include "Define.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#else
#include <uuid/uuid.h>  // need -luuid
#endif

NAMESPACE_ZL_UTIL_START

std::string createUuid()
{
    char buf[64] = {0};
#ifdef OS_WINDOWS
    GUID guid;
    if (CoCreateGuid(&guid) == S_OK){
        ZL_SNPRINTF(buf, sizeof(buf),
            "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
    }
#else
    uuid_t uu;
    uuid_generate(uu);
    if (!uuid_is_null(uu)) {
        ZL_SNPRINTF(buf, sizeof(buf),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7], 
            uu[8], uu[9], uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
    }
#endif

    return buf;
}

NAMESPACE_ZL_UTIL_END
#endif  /* ZL_UUID_H */