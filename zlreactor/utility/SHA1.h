// ***********************************************************************
// Filename         : SHA1.h
// Author           : LIZHENG
// Created          : 2015--27
// Description      : 安全哈希算法（Secure Hash Algorithm）
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-11-10
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SHA1_H
#define ZL_SHA1_H
#include <string>
#include <stdint.h>
// 对于长度小于2^64位的消息，SHA1会产生一个160位的消息摘要。当接收到消息的时候，这个消息摘要可以用来验证数据的完整性。
// 在传输的过程中，数据很可能会发生变化，那么这时候就会产生不同的消息摘要。
// SHA1特性：不可以从消息摘要中复原信息；两个不同的消息不会产生同样的消息摘要。

namespace zl
{
    namespace util
    {

        class SHA1
        {
        public:
            SHA1();
            ~SHA1();

            static std::string hexDigest(const std::string& src);

        public:
            void reset();
            void update(const std::string& sp);
            // Finalizes the Sha1 operation and fills the buffer with the digest.
            //  Data is uint8_t digest_[20]
            void final(void* digest);
            //  Hex encoding for result
            std::string hexFinal();

        private:
            void sha1Transform(uint32_t state[5], const uint8_t buffer[64]);
            void update(const uint8_t* data, size_t input_len);
            void finalInternal();

        private:
            SHA1(const SHA1&);
            const SHA1& operator=(const SHA1&);

            struct SHA1_CTX
            {
                uint32_t state[5];
                uint32_t count[2];  // Bit count of input.
                uint8_t buffer[64];
            };

            SHA1_CTX context_;
            uint8_t  digest_[20];
        };

    }
}
#endif  /* ZL_SHA1_H */