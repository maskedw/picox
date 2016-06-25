/**
 *       @file  xutils.c
 *      @brief  低レベルなあれこれ雑多なユーティリティ集
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/06/20
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <picox/core/xcore.h>


#define X_DECLARE_BIT_REVERSE_TABLE       \
    const uint8_t bit_reverse_table[16] = \
        {                                 \
            0,  /* 0000 -> 0000 */        \
            8,  /* 0001 -> 1000 */        \
            4,  /* 0010 -> 0100 */        \
            12, /* 0011 -> 1100 */        \
            2,  /* 0100 -> 0010 */        \
            10, /* 0101 -> 1010 */        \
            6,  /* 0110 -> 0110 */        \
            14, /* 0111 -> 1110 */        \
            1,  /* 1000 -> 0001 */        \
            9,  /* 1001 -> 1001 */        \
            5,  /* 1010 -> 0101 */        \
            13, /* 1011 -> 1101 */        \
            3,  /* 1100 -> 0011 */        \
            11, /* 1101 -> 1011 */        \
            7,  /* 1110 -> 0111 */        \
            15, /* 1111 -> 1111 */        \
        }

#define X_DECLARE_LSB_POS_TABLE            \
    const uint8_t lsb_pos_table[15] =      \
        {                                  \
            0, /* (0001 & 1 << 0) != 0 */  \
            1, /* (0010 & 1 << 1) != 0 */  \
            0, /* (0011 & 1 << 0) != 0 */  \
            2, /* (0100 & 1 << 2) != 0 */  \
            0, /* (0101 & 1 << 0) != 0 */  \
            1, /* (0110 & 1 << 1) != 0 */  \
            0, /* (0111 & 1 << 0) != 0 */  \
            3, /* (1000 & 1 << 3) != 0 */  \
            0, /* (1001 & 1 << 0) != 0 */  \
            1, /* (1010 & 1 << 1) != 0 */  \
            0, /* (1011 & 1 << 0) != 0 */  \
            2, /* (1100 & 1 << 2) != 0 */  \
            0, /* (1101 & 1 << 0) != 0 */  \
            1, /* (1110 & 1 << 1) != 0 */  \
            0, /* (1111 & 1 << 0) != 0 */  \
        }
#define X_DECLARE_MSB_POS_TABLE            \
const uint8_t msb_pos_table[15] =          \
    {                                      \
        0,  /* (0001 & 1 << 0) != 0 */     \
        1,  /* (0010 & 1 << 1) != 0 */     \
        1,  /* (0011 & 1 << 1) != 0 */     \
        2,  /* (0100 & 1 << 2) != 0 */     \
        2,  /* (0101 & 1 << 2) != 0 */     \
        2,  /* (0110 & 1 << 2) != 0 */     \
        2,  /* (0111 & 1 << 2) != 0 */     \
        3,  /* (1000 & 1 << 3) != 0 */     \
        3,  /* (1001 & 1 << 3) != 0 */     \
        3,  /* (1010 & 1 << 3) != 0 */     \
        3,  /* (1011 & 1 << 3) != 0 */     \
        3,  /* (1100 & 1 << 3) != 0 */     \
        3,  /* (1101 & 1 << 3) != 0 */     \
        3,  /* (1110 & 1 << 3) != 0 */     \
        3,  /* (1111 & 1 << 3) != 0 */     \
    }


uint32_t x_roundup_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDUP_MULTIPLE(x, m);
}


void* x_roundup_multiple_ptr(const void* x, uint32_t m)
{
    return (void*)X_ROUNDUP_MULTIPLE_PTR(x, m);
}


uint32_t x_rounddown_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDDOWN_MULTIPLE(x, m);
}


void* x_rounddown_multiple_ptr(const void* x, uintptr_t m)
{
    return (void*)X_ROUNDDOWN_MULTIPLE_PTR(x, m);
}


uint32_t x_roundup_power_of_two(uint32_t x)
{
    return X_ROUNDUP_POWER_OF_TWO(x);
}


void* x_roundup_power_of_two_ptr(const void* x)
{
    return (void*)X_ROUNDUP_POWER_OF_TWO_PTR(x);
}


uint32_t x_rounddown_power_of_two(uint32_t x)
{
    return X_ROUNDDOWN_POWER_OF_TWO(x);
}


void* x_rounddown_power_of_two_ptr(const void* x)
{
    return X_ROUNDDOWN_POWER_OF_TWO_PTR(x);
}


bool x_is_multiple(uint32_t x, uint32_t m)
{
    return X_IS_MULTIPLE(x, m);
}


bool x_is_multiple_ptr(const void* x, uint32_t m)
{
    return X_IS_MULTIPLE_PTR(x, m);
}


bool x_is_power_of_two(uint32_t x)
{
    return X_IS_POWER_OF_TWO(x);
}


bool x_is_power_of_two_ptr(const void* x)
{
    return X_IS_POWER_OF_TWO_PTR(x);
}


uint32_t x_roundup_alignment(uint32_t x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDUP_ALIGNMENT(x, a);
}


void* x_roundup_alignment_ptr(const void* x, size_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDUP_ALIGNMENT_PTR(x, a);
}


uint32_t x_rounddown_alignment(uint32_t x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDDOWN_ALIGNMENT(x, a);
}


void* x_rounddown_alignment_ptr(const void* x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDDOWN_ALIGNMENT_PTR(x, a);
}


bool x_is_alignment(uint32_t x)
{
    return X_IS_ALIGNMENT(x);
}


bool x_is_aligned(const void* ptr, size_t alignment)
{
    const bool ok = (X_ROUNDUP_ALIGNMENT_PTR(ptr, alignment) == ptr);
    X_ASSERT(X_IS_POWER_OF_TWO(alignment));
    return ok;
}


uint8_t x_reverse_bits8(uint8_t x)
{
    X_DECLARE_BIT_REVERSE_TABLE;
    const uint8_t a = bit_reverse_table[(x >> 4)];
    const uint8_t b = (uint8_t)(bit_reverse_table[(x & 0xf)] << 4);
    return a | b;
}


uint16_t x_reverse_bits16(uint16_t x)
{
    const uint16_t a = x_reverse_bits8((uint8_t)(x >> 8));
    const uint16_t b = (uint16_t)(x_reverse_bits8((uint8_t)x)) << 8;
    return a | b;
}


uint32_t x_reverse_bits32(register uint32_t x)
{
    const uint32_t a = x_reverse_bits16((uint16_t)(x >> 16));
    const uint32_t b = ((uint32_t)x_reverse_bits16((uint16_t)x)) << 16;
    return a | b;
}


uint16_t x_reverse_endian16(register uint16_t x)
{
    return X_REVERSE_ENDIAN16(x);
}


uint32_t x_reverse_endian32(uint32_t x)
{
    return X_REVERSE_ENDIAN32(x);
}


ptrdiff_t x_distance_ptr(const void* begin, const void* end)
{
    return (const char*)(end) - (const char*)(begin);
}


bool x_is_within(int32_t x, int32_t begin, int32_t end)
{
    return ((begin <= x) && (x < end));
}


bool x_is_uwithin(uint32_t x, uint32_t begin, uint32_t end)
{
    return ((begin <= x) && (x < end));
}


bool x_is_within_uptr(uintptr_t x, uintptr_t begin, uintptr_t end)
{
    return ((begin <= x) && (x < end));
}


bool x_is_within_ptr(const void* ptr, const void* begin, const void* end)
{
    const char* const p = (const char*)(ptr);
    const char* const b = (const char*)(begin);
    const char* const e = (const char*)(end);

    return ((b <= p) && (p < e));
}


int x_find_lsb_pos8(uint8_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


int x_find_lsb_pos16(uint16_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x00ff))     { x >>=  8; n +=  8;}
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


int x_find_lsb_pos32(uint32_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x0000ffff)) { x >>= 16; n += 16;}
    if (!(x & 0x00ff))     { x >>=  8; n +=  8;}
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


uint8_t  x_find_lsb8(uint8_t x)   { return x & (~x + 1); }
uint16_t x_find_lsb16(uint16_t x) { return x & (~x + 1); }
uint32_t x_find_lsb32(uint32_t x) { return x & (~x + 1); }


int x_find_msb_pos8(uint8_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


int x_find_msb_pos16(uint16_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xff00)     { x >>=  8; n +=  8;}
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


int x_find_msb_pos32(uint32_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xffff0000) { x >>= 16; n += 16;}
    if (x & 0xff00)     { x >>=  8; n +=  8;}
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


uint8_t  x_find_msb8(uint8_t x)   { return 1U  << x_find_msb_pos8(x); }
uint16_t x_find_msb16(uint16_t x) { return 1U  << x_find_msb_pos16(x); }
uint32_t x_find_msb32(uint32_t x) { return 1UL << x_find_msb_pos32(x); }


/// @cond IGNORE
#define X_COUNT_BITS_IMPL(x, type) int count = 0; while(x) { ++count; x = (type)(x & (x - 1)); } return count
/// @endcond IGNORE

int x_count_bits8(uint8_t x)  { X_COUNT_BITS_IMPL(x, uint8_t); }
int x_count_bits16(uint16_t x) { X_COUNT_BITS_IMPL(x, uint16_t); }
int x_count_bits32(uint32_t x) { X_COUNT_BITS_IMPL(x, uint32_t); }


void x_reverse_2byte(void* x)
{
    uint8_t* p = (uint8_t*)(x);
    uint8_t tmp;
    tmp    = *p;
    *p     = *(p+1);
    *(p+1) = tmp;
}


void x_reverse_4byte(void* x)
{
    uint8_t* p = (uint8_t*)(x);
    uint8_t tmp;
    tmp    = *p;
    *p     = *(p+3);
    *(p+3) = tmp;
    tmp    = *(p+1);
    *(p+1) = *(p+2);
    *(p+2) = tmp;
}


#if defined(X_COMPILER_NO_64BIT_INT)
int32_t x_map(
        int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return ((int32_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#else
int32_t x_map(
        int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return ((int64_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif


#if defined(X_COMPILER_NO_64BIT_INT)
uint32_t x_umap(
        uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return ((uint32_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#else
uint32_t x_umap(
        uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return ((uint64_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif


bool x_is_big_endian(void)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return true;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return false;
#else
    union
    {
        uint32_t u;
        uint8_t  c[4];
    } e = {0x01000000};
    return e.c[0];
#endif
}


bool x_is_little_endian(void)
{
    return ! x_is_big_endian();
}


uint16_t x_big_to_host16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian16(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian16(x);
#endif
}


uint32_t x_big_to_host32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian32(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian32(x);
#endif
}


uint16_t x_little_to_host16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian16(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian16(x);
#endif
}


uint32_t x_little_to_host32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian32(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian32(x);
#endif
}


uint16_t x_host_to_big16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian16(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian16(x);
#endif
}


uint32_t x_host_to_big32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian32(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian32(x);
#endif
}


uint16_t x_host_to_little16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian16(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian16(x);
#endif
}


uint32_t x_host_to_little32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian32(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian32(x);
#endif
}
