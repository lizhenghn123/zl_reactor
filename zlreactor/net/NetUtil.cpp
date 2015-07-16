#include "NetUtil.h"
#include <stdlib.h>
#include <string.h>
NAMESPACE_ZL_NET_START

bool NetUtil::isBroadcastAddress(const char *str)
{
    return (NULL == str) ? false : (0 == strcmp(str, "255.255.255.255"));
}

bool NetUtil::isValidIp(const char *str)
{
    return isValidIpv4(str) || isValidIpv6(str);
}

bool NetUtil::isValidIpv4(const char *str)
{
    if((NULL == str) || (0 == str[0]) || ('0' == str[0])) return false;
    if(0 == strcmp(str, "*")) return true;
    if(0 == strcmp(str, "255.255.255.255")) return false;

    int dot = 0; // .的个数
    const char *strp = str;
    int num = 0; //计算每一个以'.'分开的字符串数值
    while(*strp)
    {
        if('.' == *strp)
        {
            ++dot;
            num = 0;
        }
        else if((*strp < '0') || (*strp > '9'))
        {
            return false;
        }
        else
        {
            num *= 10;
            num += (*strp - '0');
            if(num > 255)
                return false;
        }
        ++strp;
    }

    return (3 == dot);  // .的个数必须为3
}

bool NetUtil::isValidIpv6(const char *str)
{
    const char *pos = strchr(str, ':');
    if(NULL == pos) return false;
    return strchr(pos, ':') != NULL;
}

bool NetUtil::isLittleEndian()
{
    //Little-endian模式的CPU对操作数的存放方式是从低字节到高字节，而Big-endian模式对操作数的存放方式是从高字节到低字节。
    //[大小端存储问题]: 小端方式中（i占至少两个字节的长度）,i所分配的内存最小地址那个字节中就存着1，其他字节是0.
    //大端的话则1在i的最高地址字节处存放，char是一个字节，所以强制将char型量p指向i则p指向的一定是i的最低地址，
    //那么就可以判断p中的值是不是1来确定是不是小端。
    //若处理器是Big_endian的，则返回0；若是Little_endian的，则返回1

    //联合体union的存放顺序是所有成员都从低地址开始存放
    union w
    {
        int i;
        char c;
    } u;
    u.i = 1;

    return (u.c == 1);

    //int i = 0x1;
    //return *(char *)&i == 0x1;  // this is ok
}

void NetUtil::reverseBytes(const void *source, void *result, size_t length)
{
    char *source_begin = (char *)source;
    char *result_end = ((char *)result) + length;

    for(size_t i = 0; i < length; ++i)
        *(--result_end) = source_begin[i];
}

void NetUtil::host2Net(const void *source, void *result, size_t length)
{
    if(isLittleEndian())   //只有小字节序才需要转换，大字节序和网络字节序是一致的
        reverseBytes(source, result, length);
}

void NetUtil::net2Host(const void *source, void *result, size_t length)
{
    host2Net(source, result, length);
}

NAMESPACE_ZL_NET_END