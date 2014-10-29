#include "ByteArray.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ByteArray::ByteArray(int allocSize)
{
    writePos_ = 0;
    readPos_ = 0;
    bytesBuf_.reserve(allocSize <= 0 ? 1 : allocSize);
    bytesBuf_.clear();
    Endian cpuEndian = getCPUEndian();
    setEndian(cpuEndian, cpuEndian);
}

ByteArray::~ByteArray()
{
    std::vector<char>().swap(bytesBuf_);
}

void ByteArray::writeBool(bool val)
{
    writeChars((char *)&val, 1, 0);
}

void ByteArray::writeByte(char val)
{
    writeChars(&val, 1);
}

void ByteArray::writeChars(const char *val)
{
    writeChars(val, (int)strlen(val), 0);
}

void ByteArray::writeString(const std::string& val)
{
    writeNumber(val.size());
    writeChars(val.c_str(), val.size(), 0);
}

void ByteArray::writeChars(const char *val, size_t size, int offset/* = 0*/)
{
    const char *srcByte = val + offset;
    if((int)bytesBuf_.size() < (writePos_ + size))
    {
        bytesBuf_.resize(writePos_ + size);  //每次只按需扩展所需大小，避免多分配内存, 也可直接resize 2倍大小，就像stl那样
    }
    ::memcpy(&bytesBuf_[writePos_], srcByte, size);
    writePos_ += size;
}

bool ByteArray::readBool()
{
    bool val = false;
    readBytes((char *)&val, 1, 0);
    return val;
}

char ByteArray::readByte()
{
    char val;
    readBytes(&val, 1);
    return val;
}

bool ByteArray::readBytes(char *val, size_t size, int offset/* = 0*/)
{
    char *dstByte = val + offset;
    if(readPos_ + size > (int)bytesBuf_.size())
    {
        *dstByte = 0;
        return false;
    }
    ::memcpy(dstByte, &bytesBuf_[readPos_], size);
    readPos_ += size;
    return true;
}

void ByteArray::readChars(char *val, size_t size)
{
    readBytes((char *)val, size, 0);
}

std::string ByteArray::readString()
{
    int size;
    readNumber(&size);
    char *chars = (char *)malloc(size + 1);
    memset(chars, 0, size + 1);
    readBytes((char *)chars, size, 0);
    std::string str(chars);
    free(chars);
    return str;
}

Endian ByteArray::getCPUEndian()
{
    union w
    {
        int i;
        char c;
    } u;
    u.i = 1;

    if(u.c == 1)
        return LITENDIAN;
    else
        return BIGENDIAN;
}

void ByteArray::reversalArray(char *bytes, size_t size)
{
    for(size_t i = 0; i < size / 2; i++)
    {
        char tb;
        tb = *(bytes + i);
        *(bytes + i) = *(bytes + size - i - 1);
        *(bytes + size - i - 1) = tb;
    }
}

