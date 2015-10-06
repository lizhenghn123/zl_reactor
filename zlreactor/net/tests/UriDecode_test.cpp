#include <iostream>
#include <string.h>
#include <assert.h>
#include <string>
#include "net/http/UriUtil.h"
#include "UriDecode_gbk.h"
#include "UriDecode_utf8.h"
using namespace std;

//void url_decode(char* src, char* dest, int max)
//{
//    char *p = src;
//    char code[3] = { 0 };
//    while(*p && --max)
//    {
//        if(*p == '%')
//        {
//            memcpy(code, ++p, 2);
//            *dest++ = (char)strtoul(code, NULL, 16);
//            p += 2;
//        }
//        else
//        {
//            *dest++ = *p++;
//        }
//    }
//    *dest = '\0';
//}

void test_UriDecode(const char* encoded)
{
    char dst[1024] = { 0 };
    zl::net::uriDecode(encoded, strlen(encoded), dst);
    cout << dst << "\n";
    cout << zl::net::uriDecode(encoded) << "\n";
    cout << zl::net::uriDecode(encoded, strlen(encoded)) << "\n";
}

void test_UriEncode(const char* unencoded)
{
    char dst[1024] = { 0 };
    zl::net::uriEncode(unencoded, strlen(unencoded), dst);
    cout << dst << "\n";
    cout << zl::net::uriEncode(unencoded) << "\n";
    cout << zl::net::uriEncode(unencoded, strlen(unencoded)) << "\n";
}

#define ZL_EXCEPT_EQ(x, y)  assert(std::string(x) == std::string(y))

// http://tool.chinaz.com/Tools/URLEncode.aspx
int main()
{
    ZL_EXCEPT_EQ("percent%20encoding", zl::net::uriEncode("percent encoding"));
    ZL_EXCEPT_EQ("percent encoding", zl::net::uriDecode("percent+encoding"));

    ZL_EXCEPT_EQ(kTigerGBKEncoded, zl::net::uriEncode(kTigerGBK));
    ZL_EXCEPT_EQ(kTigerGBK, zl::net::uriDecode(kTigerGBKEncoded));

    ZL_EXCEPT_EQ(kTigerUtf8Encoded, zl::net::uriEncode(kTigerUtf8));
    ZL_EXCEPT_EQ(kTigerUtf8, zl::net::uriDecode(kTigerUtf8Encoded));

    cout << "#########  test_UriDecode  #########\n";
    {
        char str[] = { "http://www.baidu.com/rr?%2Bfe" };
        test_UriDecode(str);
        cout << "---------------\n";
    }
    {
        char str[] = { "http://192.168.9.2:8080/bank/parser?line=%E6%9F%A5%E4%BD%99%E9%A2%9D&userid=795" };
        test_UriDecode(str);
        cout << "---------------\n";
    }
    {
        char str[] = { "http://192.168.9.2:8080/bank/parser?line=²éÓà¶î&userid=795" };
        test_UriDecode(str);
        cout << "---------------\n";
    }
    {
        char str[] = { "http%3A%2F%2F192.168.9.2%3A8080%2Fbank%2Fparser%3Fline%3D%B2%E9%D3%E0%B6%EE%26userid%3D795" };
        test_UriDecode(str);
        cout << "---------------\n";
    }

    cout << "#########  test_UriEncode  #########\n";
    {
        char str[] = { "http://192.168.14.7:9999/sys/cpuinfo" };
        test_UriEncode(str);
        cout << "---------------\n";
    }
    return 0;
}

