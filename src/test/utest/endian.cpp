/*
 * endian.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <test/utest.h>
#include <test/ByteBuffer.h>

static uint8_t u8_ex[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        };

static uint8_t u16_ex[] = {
            0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06,
            0x09, 0x08, 0x0b, 0x0a, 0x0d, 0x0c, 0x0f, 0x0e,
            0x11, 0x10, 0x13, 0x12, 0x15, 0x14, 0x17, 0x16,
            0x19, 0x18, 0x1b, 0x1a, 0x1d, 0x1c, 0x1f, 0x1e,
            0x21, 0x20, 0x23, 0x22, 0x25, 0x24, 0x27, 0x26,
            0x29, 0x28, 0x2b, 0x2a, 0x2d, 0x2c, 0x2f, 0x2e,
            0x31, 0x30, 0x33, 0x32, 0x35, 0x34, 0x37, 0x36,
            0x39, 0x38, 0x3b, 0x3a, 0x3d, 0x3c, 0x3f, 0x3e
        };

static uint8_t u32_ex[] = {
            0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04,
            0x0b, 0x0a, 0x09, 0x08, 0x0f, 0x0e, 0x0d, 0x0c,
            0x13, 0x12, 0x11, 0x10, 0x17, 0x16, 0x15, 0x14,
            0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c,
            0x23, 0x22, 0x21, 0x20, 0x27, 0x26, 0x25, 0x24,
            0x2b, 0x2a, 0x29, 0x28, 0x2f, 0x2e, 0x2d, 0x2c,
            0x33, 0x32, 0x31, 0x30, 0x37, 0x36, 0x35, 0x34,
            0x3b, 0x3a, 0x39, 0x38, 0x3f, 0x3e, 0x3d, 0x3c
        };

static uint8_t u64_ex[] = {
            0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
            0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
            0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
            0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18,
            0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
            0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28,
            0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
            0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38
        };

UTEST_BEGIN("dsp", endian)
    template <typename T>
        void call(const char *label, const uint8_t *ethalon)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                for (size_t n=0; n <= sizeof(u8_ex)/sizeof(T); ++n)
                {
                    printf("Testing %s for %d elemens\n", label, int(n));

                    ByteBuffer dst(u8_ex, n * sizeof(T), DEFAULT_ALIGN, mask & 0x01);
                    byte_swap(dst.data<T>(), n);

                    UTEST_ASSERT_MSG(dst.valid(), "Dest buffer corrupted");
                    UTEST_ASSERT_MSG(dst.equals(ethalon, n*sizeof(T)), "Byte swap failed");
                }
            }
        }

    UTEST_MAIN
    {
        UTEST_ASSERT(byte_swap(uint8_t(0xa5)) == uint8_t(0xa5));
        UTEST_ASSERT(byte_swap(int8_t(0xa5)) == int8_t(0xa5));
        UTEST_ASSERT(byte_swap(uint16_t(0xa55a)) == uint16_t(0x5aa5));
        UTEST_ASSERT(byte_swap(int16_t(0xa55a)) == int16_t(0x5aa5));
        UTEST_ASSERT(byte_swap(uint32_t(0x01020304)) == uint32_t(0x04030201));
        UTEST_ASSERT(byte_swap(int32_t(0x01020304)) == int32_t(0x04030201));
        UTEST_ASSERT(byte_swap(uint64_t(0x0102030405060708LL)) == uint64_t(0x0807060504030201LL));
        UTEST_ASSERT(byte_swap(int64_t(0x0102030405060708LL)) == int64_t(0x0807060504030201LL));

        call<uint8_t>("bswap u8", u8_ex);
        call<int8_t>("bswap i8", u8_ex);
        call<uint16_t>("bswap u16", u16_ex);
        call<int16_t>("bswap i16", u16_ex);
        call<uint32_t>("bswap u32", u32_ex);
        call<int32_t>("bswap i32", u32_ex);
        call<uint64_t>("bswap u64", u64_ex);
        call<int64_t>("bswap i64", u64_ex);
    }

UTEST_END;

