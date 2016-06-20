/**
 *       @file  stdint.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/06/17
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and self permission notice shall be
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
#ifndef stdint_h_
#define stdint_h_


typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed long         int32_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned long       uint32_t;

typedef int8_t              int_least8_t;
typedef int16_t             int_least16_t;
typedef int32_t             int_least32_t;

typedef uint8_t             uint_least8_t;
typedef uint16_t            uint_least16_t;
typedef uint32_t            uint_least32_t;

typedef int8_t              int_fast8_t;
typedef int16_t             int_fast16_t;
typedef int32_t             int_fast32_t;

typedef uint8_t             uint_fast8_t;
typedef uint16_t            uint_fast16_t;
typedef uint32_t            uint_fast32_t;

typedef int32_t             intptr_t;
typedef uint32_t            uintptr_t;

typedef int32_t             intmax_t;
typedef uint32_t            uintmax_t;

#define INT8_MIN            (-0x7F - 1)
#define INT16_MIN           (-0x7FFF - 1)
#define INT32_MIN           (-0x7FFFFFFF - 1)

#define INT8_MAX            (0x7F)
#define INT16_MAX           (0x7FFF)
#define INT32_MAX           (0x7FFFFFFF)

#define UINT8_MAX           (0xFFU)
#define UINT16_MAX          (0xFFFFU)
#define UINT32_MAX          (0xFFFFFFFFU)

#define INT_LEAST8_MIN      INT8_MIN
#define INT_LEAST16_MIN     INT16_MIN
#define INT_LEAST32_MIN     INT32_MIN

#define INT_LEAST8_MAX      INT8_MAX
#define INT_LEAST16_MAX     INT16_MAX
#define INT_LEAST32_MAX     INT32_MAX

#define UINT_LEAST8_MAX     UINT8_MAX
#define UINT_LEAST16_MAX    UINT16_MAX
#define UINT_LEAST32_MAX    UINT32_MAX

#define INT_FAST8_MIN       INT_LEAST8_MIN
#define INT_FAST16_MIN      INT_LEAST16_MIN
#define INT_FAST32_MIN      INT_LEAST32_MIN

#define INT_FAST8_MAX       INT_LEAST8_MAX
#define INT_FAST16_MAX      INT_LEAST16_MAX
#define INT_FAST32_MAX      INT_LEAST32_MAX

#define UINT_FAST8_MAX      UINT_LEAST8_MAX
#define UINT_FAST16_MAX     UINT_LEAST16_MAX
#define UINT_FAST32_MAX     UINT_LEAST32_MAX

#define INTPTR_MIN          INT32_MIN
#define INTPTR_MAX          INT32_MAX
#define UINTPTR_MAX         UINT32_MAX

#define INTMAX_MIN          INT32_MIN
#define INTMAX_MAX          INT32_MAX
#define UINTMAX_MAX         UINT32_MAX

#define PTRDIFF_MIN         INT16_MIN
#define PTRDIFF_MAX         INT16_MAX

#define SIZE_MAX            UINT16_MAX

#define INT8_C(x)           (x)
#define INT16_C(x)          (x)
#define INT32_C(x)          (x ## L)

#define UINT8_C(x)          (x)
#define UINT16_C(x)         (x ## U)
#define UINT32_C(x)         (x ## UL)

#define INTMAX_C(x)         INT32_C(x)
#define UINTMAX_C(x)        UINT32_C(x)


#endif /* stdint_h_ */
