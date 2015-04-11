// ***********************************************************************
// Filename         : MathUtil.h
// Author           : LIZHENG
// Created          : 2014-11-02
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-11-02
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STOPWTACH_H
#define ZL_STOPWTACH_H
#include "Define.h"
NAMESPACE_ZL_BASE_START

/// 判断一个整数是否是2的幂次方
inline bool is_pow2(uint32_t num)
{
    return !(num & (num - 1));
}

/// 返回一个整数，其值不小于参数num，且为2的幂次方
inline uint32_t num_of_pow2(uint32_t num)
{
    if(is_pow2(num)) 
        return num;
    num = num-1;
    num = num | (num>>1);
    num = num | (num>>2);
    num = num | (num>>4);
    num = num | (num>>8);
    num = num | (num>>16);
    return num + 1;
}

/// 返回一个不小于参数num的整数的关于2的幂次
inline uint8_t get_pow2(uint32_t num)
{
    uint8_t pow2 = 0;
    if(!is_pow2(num)) 
        num = (num << 1);
    while(num > 1)
    {
        pow2++;
        num = num >> 1;
    }
    return pow2;
}

NAMESPACE_ZL_BASE_END