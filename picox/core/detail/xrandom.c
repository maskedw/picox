/**
 *       @file  xrandom.c
 *      @brief  擬似乱数生成実装
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/03/13
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


static uint8_t xor8 = 1;
static uint16_t xor16 = 1;
static uint32_t xor32 = 1;


void x_srand(uint32_t seed)
{
    xor8 = (uint8_t)seed;
    xor16 = (uint16_t)seed;
    xor32 = seed;

    /* 以下参考リンクによるとseedは0以外なら何でもいいらしい。
     * hexadrive.sblo.jp/article/63660775.html
     */
    if (!xor8) xor8 = 1;
    if (!xor16) xor16 = 1;
    if (!xor32) xor32 = 1;
}


uint8_t x_rand8(void)
{
    /* http://www.arklyffe.com/main/2010/08/29/xorshift-pseudorandom-number-generator/ */
    xor8 ^= (uint8_t)(xor8 << 7);
    xor8 ^= (uint8_t)(xor8 >> 5);
    return xor8 ^= (uint8_t)(xor8 << 3);
}


uint16_t x_rand16(void)
{
    /* http://www.arklyffe.com/main/2010/08/29/xorshift-pseudorandom-number-generator/ */
    xor16 ^= (xor16 << 13);
    xor16 ^= (xor16 >> 9);
    return xor16 ^= (xor16 << 7);
}


uint32_t x_rand32(void)
{
    /* https://ja.wikipedia.org/wiki/Xorshift */
    xor32 ^= (xor32 << 13);
    xor32 ^= (xor32 >> 17);
    xor32 ^= (xor32 << 15);
    return xor32;
}


unsigned x_rand(void)
{
#if X_SIZEOF_INT == 1
    return x_rand8();
#elif X_SIZEOF_INT == 2
    return x_rand16();
#else
    return x_rand32();
#endif
}


uint8_t x_randrange8(uint8_t min, uint8_t max)
{
    const uint8_t v = x_rand8();
    return min + v % (max - min);
}


uint16_t x_randrange16(uint16_t min, uint16_t max)
{
    const uint16_t v = x_rand16();
    return min + v % (max - min);
}


uint32_t x_randrange32(uint32_t min, uint32_t max)
{
    const uint32_t v = x_rand32();
    return min + v % (max - min);
}


unsigned x_randrange(unsigned min, unsigned max)
{
#if X_SIZEOF_INT == 1
    return x_randrange8(min, max);
#elif X_SIZEOF_INT == 2
    return x_randrange16(min, max);
#else
    return x_randrange32(min, max);
#endif
}
