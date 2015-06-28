/**
 *       @file  memory.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/28
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_core_detail_memory_h_
#define picox_core_detail_memory_h_


#ifdef __cplusplus
extern "C" {
#endif


#ifdef X_CONF_MALLOC
    #define X_MALLOC    X_CONF_MALLOC
#else
    #define X_MALLOC    malloc
#endif


#ifdef X_CONF_FREE
    #define X_FREE      X_CONF_FREE
#else
    #define X_FREE      free
#endif


#ifdef  X_CONF_USE_DETECT_MALLOC_NULL
    #define X_ASSERT_MALLOC_NULL(expr)    X_ASSERT(expr)
#else
    #define X_ASSERT_MALLOC_NULL(expr)    do { if (!(expr)) return NULL; } while (0)
#endif


#define X_SAFE_FREE(ptr)  (X_FREE((ptr)), (ptr) = NULL)


#ifdef __cplusplus
}
#endif


#endif // picox_core_detail_memory_h_
