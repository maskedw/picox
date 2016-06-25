/**
 *       @file  limits.h
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
#ifndef limits_h_
#define limits_h_


#define CHAR_BIT            (8)
#ifndef __CHAR_UNSIGNED__
#define CHAR_MAX            (+127)
#define CHAR_MIN            (-128)
#else   /* !__CHAR_UNSIGNED__ */
#define CHAR_MAX            (255U)
#define CHAR_MIN            (0)
#endif  /* !__CHAR_UNSIGNED__ */
#define SCHAR_MAX           (+127)
#define SCHAR_MIN           (-128)
#define UCHAR_MAX           (255U)
#define INT_MAX             (+32767)
#define INT_MIN             (-32768)
#define SHRT_MAX            (+32767)
#define SHRT_MIN            (-32768)
#define UINT_MAX            (65535U)
#define USHRT_MAX           (65535U)
#define SINT_MAX            (+32767)
#define SINT_MIN            (-32768)
#define SSHRT_MAX           (+32767)
#define SSHRT_MIN           (-32768)
#define LONG_MAX            (+2147483647)
#define LONG_MIN            (-2147483648)
#define ULONG_MAX           (4294967295U)


#endif /* limits_h_ */
