/**
 *       @file  xmessage_buffer.h
 *      @brief  可変長メッセージを格納するコンテナです
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/13
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of uint8_tge, to any person
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


#ifndef xmessage_buffer_h_
#define xmessage_buffer_h_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#ifndef XMBUF_ASSERT

    #define XMBUF_ASSERT(expr)   do { if (! expr) for(;;); } while (0)

#endif


/** メッセージヘッダ
 *
 *  XMBufへnバイトのデータを格納する時、XMBufの空き容量はsizeof(XMBufHeader) + n
 *  バイト以上必要です。
 *  ユーザーはsizeof(XMBufHeader)以外でこの型を使用する必要はありません。
 */
typedef union XMBufHeader
{
    size_t  size;
    uint8_t bytes[sizeof(size_t)];
} XMBufHeader;


/** Message Buffer
 *
 *  @note
 *  メッセージはリングバッファ形式で格納されます。
 */
typedef struct XMBuf
{
/// @privatesection
    uint8_t*    data;       /** メッセージ格納先 */
    size_t      first;      /** 読み込みインデックス */
    size_t      last;       /** 書き込みインデックス */
    size_t      size;       /** dataに格納されているバイト数 */
    size_t      capacity;   /** dataのバイト数 */
} XMBuf;


/** バッファを初期化します。
 *
 *  @pre
 *  + (buf != NULL) && (size > sizeof(XMBufHeader))
 *
 *  @attention
 *  XMBufの使用中はbufが指す領域を破棄しないこと。
 */
static inline void
xmbuf_init(XMBuf* self, void* buf, size_t size)
{
    XMBUF_ASSERT(self);
    XMBUF_ASSERT(buf);
    XMBUF_ASSERT(size > sizeof(XMBufHeader));

    self->data = buf;
    self->first = self->last = self->size = 0;
    self->capacity = size;
}


/** 要素を格納するバッファを返します。
 */
static inline uint8_t*
xmbuf_data(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return self->data;
}


/** バッファを空にします。
 */
static inline void
xmbuf_clear(XMBuf* self)
{
    XMBUF_ASSERT(self);
    self->first = self->last = self->size = 0;
}


/** バッファに格納されているバイト数を返します。
 */
static inline size_t
xmbuf_size(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return self->size;
}


/** バッファが空かどうかを返します。
 */
static inline bool
xmbuf_empty(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return self->size == 0;
}


/** バッファに格納できる最大バイト数を返します。
 */
static inline size_t
xmbuf_capacity(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return self->capacity;
}


/** バッファの空きバイト数を返します。
 */
static inline size_t
xmbuf_reserve(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return self->capacity - self->size;
}


/** バッファが満タンかどうかを返します。
 *
 *  xmbuf_reserve() <= sizeof(XMBufHeader)の時も、これ以上メッセージを格納できな
 *  い為、満タンと判定します。
 */
static inline bool
xmbuf_full(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    return xmbuf_reserve(self) <= sizeof(XMBufHeader);
}


/** 先頭メッセージのバイト数を返します。
 */
static inline size_t
xmbuf_msg_size(const XMBuf* self)
{
    XMBUF_ASSERT(self);
    if (xmbuf_empty(self))
        return 0;

    XMBufHeader header;
    size_t rpos = self->first;
    int i;
    for (i = 0; i < sizeof(XMBufHeader); i++) {
        header.bytes[i] = self->data[rpos++];
        if (rpos == self->capacity)
            rpos = 0;
    }

    return header.size;
}


/** バッファ末尾にメッセージを追加します。
 *
 *  @pre
 *  + (src != NULL) && (size > 0)
 *  + xmbuf_reserve() >= size + sizeof(XMBufHeader)
 */
static inline void
xmbuf_push(XMBuf* self, const void* src, size_t size)
{
    XMBUF_ASSERT(self);
    XMBUF_ASSERT(src);
    XMBUF_ASSERT(size > 0);
    XMBUF_ASSERT(xmbuf_reserve(self) >= size + sizeof(XMBufHeader));


    XMBufHeader header;
    header.size = size;
    self->size += sizeof(XMBufHeader) + size;

    size_t pos = self->last;
    int i;
    for (i = 0; i < sizeof(XMBufHeader); i++)
    {
        self->data[pos++] = header.bytes[i];
        if (pos >= self->capacity)
            pos = 0;
    }

    const uint8_t* from = src;
    uint8_t* to;

    if (pos + size > self->capacity) {
        const size_t until_tail = self->capacity - pos;
        to = self->data + pos;
        memcpy(to, from, until_tail);

        size -= until_tail;
        from += until_tail;
        pos  = 0;
    }

    to = self->data + pos;
    memcpy(to, from, size);

    pos += size;
    if (pos == self->capacity)
        pos = 0;
    self->last = pos;
}


/** バッファ先頭からメッセージを取り出し、メッセージサイズを返します。
 *
 *  @pre
 *  + (dst != NULL)
 *  + dstが指す領域のバイト数 >= xmbuf_msg_size()
 */
static inline size_t
xmbuf_pull(XMBuf* self, void* dst)
{
    XMBUF_ASSERT(self);
    XMBUF_ASSERT(dst);


    if (xmbuf_empty(self))
        return 0;

    XMBufHeader header;
    size_t pos = self->first;
    int i;
    for (i = 0; i < sizeof(XMBufHeader); i++) {
        header.bytes[i] = self->data[pos++];
        if (pos == self->capacity)
            pos = 0;
    }

    uint8_t* to = dst;
    const uint8_t* from;
    size_t to_read = header.size;

    if (pos + to_read > self->capacity) {
        const size_t until_tail = self->capacity - pos;
        from = self->data + pos;
        memcpy(to, from, until_tail);
        to_read -= until_tail;
        to      += until_tail;
        pos     = 0;
    }

    from = self->data + pos;
    memcpy(to, from, to_read);

    pos += to_read;
    if (pos == self->capacity)
        pos = 0;
    self->first = pos;
    self->size -= header.size + sizeof(XMBufHeader);

    return header.size;
}


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // xmessage_buffer_h_
