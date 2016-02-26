/**
 *       @file  xstdlib.h
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


#ifndef picox_core_detail_xstdlib_h_
#define picox_core_detail_xstdlib_h_


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
    if (size == 0)
        return NULL;
    return X_CONF_MALLOC(size);
}


static inline void x_free(void* ptr)
{
    /* freeにNULLを渡してもOKなはずなのだが、実装によっては微妙にグレーだったり
     * するからここでもNULLチェックしておく */
    if (ptr)
        X_CONF_FREE(ptr);
}


static inline void* x_calloc(size_t nmemb, size_t size)
{
    size = nmemb * size;
    void* const ptr = x_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}


static inline void* x_realloc(void *old_mem, size_t size)
{
    void* const new_mem = x_malloc(size);
    if (! new_mem)
        return NULL;

    memcpy(new_mem, old_mem, size);
    x_free(old_mem);

    return new_mem;
}


static inline void* x_realloc2(void *old_mem, size_t old_size, size_t new_size)
{
    if (old_size == new_size)
        return old_mem;

    void* const new_mem = x_malloc(new_size);
    if (! new_mem)
        return NULL;

    /* 新しいサイズより旧いサイズの方が大きかったら新しいサイズ分コピー */
    /* 古いサイズより新しいサイズの方が大きかったら古いサイズ分コピー  */
    if (old_size > new_size)
        memcpy(new_mem, old_mem, new_size);
    else
        memcpy(new_mem, old_mem, old_size);

    x_free(old_mem);

    return new_mem;
}


#define X_SAFE_FREE(ptr)  (x_free((ptr)), (ptr) = NULL)


#ifdef __cplusplus
}
#endif


#endif // picox_core_detail_xstdlib_h_
