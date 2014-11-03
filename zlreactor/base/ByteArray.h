// ***********************************************************************
// Filename         : ByteArray.h
// Author           : LIZHENG
// Created          : 2014-09-19
// Description      : 二进制字节流
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-22
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BYTEARRAY_H
#define ZL_BYTEARRAY_H
#include "Define.h"
#include <string>
#include <vector>
NAMESPACE_ZL_BASE_START

enum Endian
{
    BIGENDIAN = 0,
    LITENDIAN = 1
};

class ByteArray
{
public:
    ByteArray(int allocSize = 64);
    ~ByteArray();

public:    //Data Write
    void writeBool(bool val);
    void writeByte(char val);
    void writeChars(const char *val);
    void writeChars(const char *val, size_t size, int offset = 0);
    void writeString(const std::string& val);

    template <typename Number>
    int writeNumber(Number val)
    {
        char bytes[sizeof(val)];
        int size = numberToBytes(val, bytes, writeEndian_);
        writeChars(bytes, size);
        return size;
    }

public:    //Data Read
    bool readBool();
    char readByte();
    void readChars(char *val, size_t size);
    std::string readString();
    bool readBytes(char *val, size_t size, int offset = 0);

    template <typename Number>
    int readNumber(Number *val)
    {
        const int size = sizeof(*val);
        char bytes[size];
        readBytes(bytes, size);
        bytesToNumber(val, bytes, readEndian_);
        return size;
    }

    template <typename Number>
    Number read()
    {
        Number val;
        const int size = sizeof(val);
        char bytes[size];
        readBytes(bytes, size);
        bytesToNumber(&val, bytes, readEndian_);
        return val;
    }

public:    //Property Access
    const char* data() const
    {
        return bytesBuf_.data();
    }

    size_t size()
    {
        return bytesBuf_.size();
    }

    size_t readableBytes() const   //还可以读取多少有效字节
    { 
        return writePos_ - readPos_; 
    }

    size_t writableBytes() const   //还可以写入有效字节
    { 
        return bytesBuf_.size() - writePos_;
    }

    void setEndian(Endian writeEndian, Endian readEndian)
    {
        writeEndian_ = writeEndian;
        readEndian_ = readEndian;
    }

private:
    template <typename T>
    int numberToBytes(T val, char *bytes, Endian endian)
    {
        int size = sizeof(val);
        *(T *)bytes = val;
        if(endian == getCPUEndian())
        {
            return size;
        }
        else
        {
            reversalArray(bytes, size);
            return size;
        }
    }
    template <typename T>
    int bytesToNumber(T val, char *bytes, Endian endian)
    {
        int size = sizeof(*val);
        if(endian == getCPUEndian())
        {
        }
        else
        {
            reversalArray(bytes, size);
        }
        *val = *(T)bytes;
        return size;
    }

    static Endian getCPUEndian();
    static void reversalArray(char *bytes, size_t size);

private:
    ByteArray(const ByteArray&);
    ByteArray& operator=(const ByteArray&);

private:
    Endian      writeEndian_;
    Endian      readEndian_;

    size_t         writePos_;
    size_t         readPos_;
    std::vector<char> bytesBuf_;
};

NAMESPACE_ZL_BASE_END
#endif /* ZL_BYTEARRAY_H */
