#include "zlreactor/utility/Base64.h"
#include <string.h>
NAMESPACE_ZL_UTIL_START

static size_t base64EncodeImpl(const char *src, size_t len, std::string& dst);
static size_t base64DecodeImpl(const char *src, size_t len, std::string& dst);

size_t base64Encode(const char *src, size_t len, char *dst)
{
    std::string dest;
    int size = base64EncodeImpl(src, len, dest);
    //strncpy(dst, dest.data(), size);
    ::memcpy(dst, dest.data(), size);
    return size;
}

size_t base64Encode(const char *src, size_t len, std::string& dst)
{
    return base64EncodeImpl(src, len, dst);
}

size_t base64Encode(const std::string& src, std::string& dst)
{
    return base64EncodeImpl(src.data(), src.size(), dst);
}

std::string    base64Encode(const char *src, size_t len)
{
    std::string dst;
    base64EncodeImpl(src, len, dst);
    return dst;
}

std::string    base64Encode(const std::string& src)
{
    std::string dst;
    base64EncodeImpl(src.data(), src.size(), dst);
    return dst;
}


size_t base64Decode(const char *src, size_t len, char *dst)
{
    std::string dest;
    int size = base64DecodeImpl(src, len, dest);
    ::memcpy(dst, dest.c_str(), size);
    return size;
}

size_t base64Decode(const char *src, size_t len, std::string& dst)
{
    return base64DecodeImpl(src, len, dst);
}

size_t base64Decode(const std::string& src, std::string& dst)
{
    return base64DecodeImpl(src.data(), src.size(), dst);
}

std::string    base64Decode(const char *src, size_t len)
{
    std::string dst;
    base64DecodeImpl(src, len, dst);
    return dst;
}

std::string    base64Decode(const std::string& src)
{
    std::string dst;
    base64DecodeImpl(src.data(), src.size(), dst);
    return dst;
}


static size_t base64EncodeImpl(const char *src, size_t len, std::string& dst)
{
    //������
    static const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char Tmp[4] = { 0 };
    int LineLength = 0;
    for (int i = 0; i<(int)(len / 3); i++)
    {
        Tmp[1] = *src++;
        Tmp[2] = *src++;
        Tmp[3] = *src++;
        dst += EncodeTable[Tmp[1] >> 2];
        dst += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        dst += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        dst += EncodeTable[Tmp[3] & 0x3F];
        if (LineLength += 4, LineLength == 76) { dst += "\r\n"; LineLength = 0; }
    }
    //��ʣ�����ݽ��б���
    int Mod = len % 3;
    if (Mod == 1)
    {
        Tmp[1] = *src++;
        dst += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        dst += EncodeTable[((Tmp[1] & 0x03) << 4)];
        dst += "==";
    }
    else if (Mod == 2)
    {
        Tmp[1] = *src++;
        Tmp[2] = *src++;
        dst += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        dst += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        dst += EncodeTable[((Tmp[2] & 0x0F) << 2)];
        dst += "=";
    }

    return dst.size();
}

static size_t base64DecodeImpl(const char *src, size_t len, std::string& dst)
{
    //������
    static const char DecodeTable[] =
    {
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   62,  0,   0,   0,  63, 
        52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  0,   0,   0,   0,   0,   0, 
        0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14, 
        15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  0,   0,   0,   0,   0, 
        0,   26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, 
        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  0,   0,   0,   0,   0
    };

    int nValue = 0;
    size_t i = 0;
    while (i < len)
    {
        if (*src != '\r' && *src != '\n')
        {
            nValue = DecodeTable[(int)*src++] << 18;
            nValue += DecodeTable[(int)*src++] << 12;
            dst += (nValue & 0x00FF0000) >> 16;
            if (*src != '=')
            {
                nValue += DecodeTable[(int)*src++] << 6;
                dst += (nValue & 0x0000FF00) >> 8;
                if (*src != '=')
                {
                    nValue += DecodeTable[(int)*src++];
                    dst += nValue & 0x000000FF;
                }
            }
            i += 4;
        }
        else   // �س�����,����
        {
            src++;
            i++;
        }
    }

    return dst.size();
}

NAMESPACE_ZL_UTIL_END