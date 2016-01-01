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


#if X_CONF_USE_DETECT_MALLOC_NULL != 0
    #define X_ASSERT_MALLOC_NULL(expr)    X_ASSERT(expr)
#else
    #define X_ASSERT_MALLOC_NULL(expr)    do { if (!(expr)) return NULL; } while (0)
#endif


static inline void* x_malloc(size_t size)
{
    return X_CONF_MALLOC(size);
}


static inline void x_free(void* ptr)
{
    /* freeにNULLを渡してもOKなはずなのだが、実装によっては微妙にグレーだったり
     * するからここでもNULLチェックしておく */
    if (ptr)
        X_CONF_FREE(ptr);
}


#define X_SAFE_FREE(ptr)  (x_free((ptr)), (ptr) = NULL)


#ifdef __cplusplus
}
#endif


#endif // picox_core_detail_memory_h_
