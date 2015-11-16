#include "SHA1.h"
#include <string.h>
namespace zl
{
    namespace util
    {

        static const int kSHA1DigestSize = 20;

        SHA1::SHA1()
        {
            reset();
        }

        SHA1::~SHA1()
        {
        }

        // SHA1Init - Initialize new context.
        void SHA1::reset()
        {
            // SHA1 initialization constants.
            context_.state[0] = 0x67452301;
            context_.state[1] = 0xEFCDAB89;
            context_.state[2] = 0x98BADCFE;
            context_.state[3] = 0x10325476;
            context_.state[4] = 0xC3D2E1F0;
            context_.count[0] = context_.count[1] = 0;
        }

        #define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

        // blk0() and blk() perform the initial expand.
        // I got the idea of expanding during the round function from SSLeay
        #define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | \
            (rol(block->l[i], 8) & 0x00FF00FF))
        #define blk(i) (block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ \
            block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

                // (R0+R1), R2, R3, R4 are the different operations used in SHA1.
        #define R0(v, w, x, y, z, i) \
            z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5A827999 + rol(v, 5); \
            w = rol(w, 30);
        #define R1(v, w, x, y, z, i) \
            z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5A827999 + rol(v, 5); \
            w = rol(w, 30);
        #define R2(v, w, x, y, z, i) \
            z += (w ^ x ^ y) + blk(i) + 0x6ED9EBA1 + rol(v, 5);\
            w = rol(w, 30);
        #define R3(v, w, x, y, z, i) \
            z += (((w | x) & y) | (w & x)) + blk(i) + 0x8F1BBCDC + rol(v, 5); \
            w = rol(w, 30);
        #define R4(v, w, x, y, z, i) \
            z += (w ^ x ^ y) + blk(i) + 0xCA62C1D6 + rol(v, 5); \
            w = rol(w, 30);

        // Hash a single 512-bit block. This is the core of the algorithm.
        void SHA1::sha1Transform(uint32_t state[5], const uint8_t buffer[64])
        {
            union CHAR64LONG16
            {
                uint8_t c[64];
                uint32_t l[16];
            };

            // Note(fbarchard): This option does modify the user's data buffer.
            CHAR64LONG16* block = const_cast<CHAR64LONG16*>(reinterpret_cast<const CHAR64LONG16*>(buffer));

            // Copy context_.state[] to working vars.
            uint32_t a = state[0];
            uint32_t b = state[1];
            uint32_t c = state[2];
            uint32_t d = state[3];
            uint32_t e = state[4];

            // 4 rounds of 20 operations each. Loop unrolled.
            // Note(fbarchard): The following has lint warnings for multiple ; on
            // a line and no space after , but is left as-is to be similar to the
            // original code.
            R0(a, b, c, d, e, 0);
            R0(e, a, b, c, d, 1);
            R0(d, e, a, b, c, 2);
            R0(c, d, e, a, b, 3);
            R0(b, c, d, e, a, 4);
            R0(a, b, c, d, e, 5);
            R0(e, a, b, c, d, 6);
            R0(d, e, a, b, c, 7);
            R0(c, d, e, a, b, 8);
            R0(b, c, d, e, a, 9);
            R0(a, b, c, d, e, 10);
            R0(e, a, b, c, d, 11);
            R0(d, e, a, b, c, 12);
            R0(c, d, e, a, b, 13);
            R0(b, c, d, e, a, 14);
            R0(a, b, c, d, e, 15);
            R1(e, a, b, c, d, 16);
            R1(d, e, a, b, c, 17);
            R1(c, d, e, a, b, 18);
            R1(b, c, d, e, a, 19);
            R2(a, b, c, d, e, 20);
            R2(e, a, b, c, d, 21);
            R2(d, e, a, b, c, 22);
            R2(c, d, e, a, b, 23);
            R2(b, c, d, e, a, 24);
            R2(a, b, c, d, e, 25);
            R2(e, a, b, c, d, 26);
            R2(d, e, a, b, c, 27);
            R2(c, d, e, a, b, 28);
            R2(b, c, d, e, a, 29);
            R2(a, b, c, d, e, 30);
            R2(e, a, b, c, d, 31);
            R2(d, e, a, b, c, 32);
            R2(c, d, e, a, b, 33);
            R2(b, c, d, e, a, 34);
            R2(a, b, c, d, e, 35);
            R2(e, a, b, c, d, 36);
            R2(d, e, a, b, c, 37);
            R2(c, d, e, a, b, 38);
            R2(b, c, d, e, a, 39);
            R3(a, b, c, d, e, 40);
            R3(e, a, b, c, d, 41);
            R3(d, e, a, b, c, 42);
            R3(c, d, e, a, b, 43);
            R3(b, c, d, e, a, 44);
            R3(a, b, c, d, e, 45);
            R3(e, a, b, c, d, 46);
            R3(d, e, a, b, c, 47);
            R3(c, d, e, a, b, 48);
            R3(b, c, d, e, a, 49);
            R3(a, b, c, d, e, 50);
            R3(e, a, b, c, d, 51);
            R3(d, e, a, b, c, 52);
            R3(c, d, e, a, b, 53);
            R3(b, c, d, e, a, 54);
            R3(a, b, c, d, e, 55);
            R3(e, a, b, c, d, 56);
            R3(d, e, a, b, c, 57);
            R3(c, d, e, a, b, 58);
            R3(b, c, d, e, a, 59);
            R4(a, b, c, d, e, 60);
            R4(e, a, b, c, d, 61);
            R4(d, e, a, b, c, 62);
            R4(c, d, e, a, b, 63);
            R4(b, c, d, e, a, 64);
            R4(a, b, c, d, e, 65);
            R4(e, a, b, c, d, 66);
            R4(d, e, a, b, c, 67);
            R4(c, d, e, a, b, 68);
            R4(b, c, d, e, a, 69);
            R4(a, b, c, d, e, 70);
            R4(e, a, b, c, d, 71);
            R4(d, e, a, b, c, 72);
            R4(c, d, e, a, b, 73);
            R4(b, c, d, e, a, 74);
            R4(a, b, c, d, e, 75);
            R4(e, a, b, c, d, 76);
            R4(d, e, a, b, c, 77);
            R4(c, d, e, a, b, 78);
            R4(b, c, d, e, a, 79);

            // Add the working vars back into context.state[].
            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
        }

        void SHA1::update(const std::string& src)
        {
            const uint8_t* buff = reinterpret_cast<const uint8_t*>(src.data());
            update(buff, src.size());
        }

        // Run your data through this.
        void SHA1::update(const uint8_t* data, size_t input_len)
        {
            size_t i = 0;

            // Compute number of bytes mod 64.
            size_t index = (context_.count[0] >> 3) & 63;

            // Update number of bits.
            // TODO(xxx): Use uint64 instead of 2 uint32_t for count.
            // count[0] has low 29 bits for byte count + 3 pad 0's making 32 bits for
            // bit count.
            // Add bit count to low uint32_t
            context_.count[0] += static_cast<uint32_t>(input_len << 3);
            if (context_.count[0] < static_cast<uint32_t>(input_len << 3))
            {
                ++context_.count[1];  // if overlow (carry), add one to high word
            }
            context_.count[1] += static_cast<uint32_t>(input_len >> 29);
            if ((index + input_len) > 63)
            {
                i = 64 - index;
                memcpy(&context_.buffer[index], data, i);
                sha1Transform(context_.state, context_.buffer);
                for (; i + 63 < input_len; i += 64)
                {
                    sha1Transform(context_.state, data + i);
                }
                index = 0;
            }
            memcpy(&context_.buffer[index], &data[i], input_len - i);
        }

        // Add padding and return the message digest.
        void SHA1::finalInternal()
        {
            uint8_t finalcount[8];
            for (int i = 0; i < 8; ++i)
            {
                // Endian independent
                finalcount[i] = static_cast<uint8_t>((context_.count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
            }
            update(reinterpret_cast<const uint8_t*>("\200"), 1);
            while ((context_.count[0] & 504) != 448)
            {
                update(reinterpret_cast<const uint8_t*>("\0"), 1);
            }
            // Should cause a SHA1Transform().
            update(finalcount, 8);
            // Wipe variables.
            memset(finalcount, 0, 8);   // SWR
        }

        void SHA1::final(void* digest)
        {
            char* data = static_cast<char*>(digest);
            finalInternal();
            for (int i = 0; i < kSHA1DigestSize; ++i)
            {
                data[i] = static_cast<uint8_t>((context_.state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
            }
        }


        static std::string encodeAsString(const void* data, size_t size, bool uppercase = false)
        {
            const char* hex_digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
            const unsigned char* p = static_cast<const unsigned char*>(data);
            const unsigned char *first = p;
            const unsigned char *end = p + size;

            std::string str;
            while (first != end)
            {
                unsigned char ch = *first;
                str.push_back(hex_digits[ch >> 4]);
                str.push_back(hex_digits[ch & 0x0F]);
                ++first;
            }

            return str;
        }

        std::string SHA1::hexFinal()
        {
            uint8_t digest[20];
            final(&digest);
            return encodeAsString(digest, 20);
        }

        std::string SHA1::hexDigest(const std::string& src)
        {
            SHA1 sha1;
            sha1.update(src);
            return sha1.hexFinal();
        }

    }
}