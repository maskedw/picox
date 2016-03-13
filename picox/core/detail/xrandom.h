/**
 *       @file  xrandom.h
 *      @brief
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


#ifndef picox_core_detail_xrandom_h_
#define picox_core_detail_xrandom_h_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void x_srand(uint32_t seed);
uint8_t x_rand8(void);
uint8_t x_randrange8(uint8_t min, uint8_t max);
uint16_t x_rand16(void);
uint16_t x_randrange16(uint16_t min, uint16_t max);
uint32_t x_rand32(void);
uint32_t x_randrange32(uint32_t min, uint32_t max);
unsigned x_rand(void);
unsigned x_randrange(unsigned min, unsigned max);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_core_detail_xrandom_h_ */
