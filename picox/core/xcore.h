/**
 *       @file  xcore.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/18
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_xcore_h_
#define picox_xcore_h_


#define X_VERSION_MAJOR     (0)
#define X_VERSION_MINOR     (6)
#define X_VERSION_STRING    "0.6"


/* 共通で使用するヘッダをまとめてインクルードする。コンパイラがプリコンパイル機
 * 能を持っているなら使用するのがコンパイル時間短縮の為に望ましい。
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <inttypes.h>
#include <picox/xconfig.h>
#include <picox/core/detail/xcompiler.h>
#include <picox/core/detail/xpreprocessor.h>
#include <picox/core/detail/xstddef.h>
#include <picox/core/detail/xdebug.h>
#include <picox/core/detail/xutils.h>
#include <picox/core/detail/xstring.h>
#include <picox/core/detail/xstdlib.h>
#include <picox/core/detail/xrtti.h>
#include <picox/core/detail/xstream.h>
#include <picox/core/detail/xstdio.h>
#include <picox/core/detail/xrandom.h>
#include <picox/core/detail/xtime.h>


#endif // picox_xcore_h_
