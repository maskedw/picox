/**
 *       @file  xfixed_memory_allocator.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/15
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


#ifndef picox_xfixed_memory_allocator_h_
#define picox_xfixed_memory_allocator_h_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#define XFALLOC_ASSERT(expr)


#ifndef XFALLOC_ALIGN
    #define XFALLOC_ALIGN   sizeof(double)
#endif


#ifndef XFALLOC_ASSERT
    #define XMBUF_ASSERT(expr)   do { if (! expr) for(;;); } while (0)
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef struct XFAlloc
{
/// privatesection
    uint8_t*    heap;
    uint8_t*    top;
    uint8_t*    next;
    size_t      block_size;
    size_t      num_blocks;
    size_t      remain_blocks;
} XFAlloc;


void xfalloc_init(XFAlloc* self, void* heap, size_t heap_size, size_t block_size);
void xfalloc_clear(XFAlloc* self);
void* xfalloc_allocate(XFAlloc* self);
void xfalloc_deallocate(XFAlloc* self, void* ptr);


#ifdef __cplusplus
}
#endif


#endif // picox_xfixed_memory_allocator_h_
