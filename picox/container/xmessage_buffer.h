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


#ifndef picox_container_xmessage_buffer_h_
#define picox_container_xmessage_buffer_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** メッセージヘッダ
 *
 *  XMessageBufferへnバイトのデータを格納する時、XMessageBufferの空き容量は
 *  sizeof(XMessageHeader) + nバイト以上必要です。
 *  ユーザーはsizeof(XMessageHeader)以外でこの型を使用する必要はありません。
 */
typedef union XMessageHeader
{
    size_t  size;
    uint8_t bytes[sizeof(size_t)];
} XMessageHeader;


/** Message Buffer
 *
 *  @note
 *  メッセージはリングバッファ形式で格納されます。
 */
typedef struct XMessageBuffer
{
/// @privatesection
    uint8_t*    data;       /** メッセージ格納先 */
    size_t      first;      /** 読み込みインデックス */
    size_t      last;       /** 書き込みインデックス */
    size_t      size;       /** dataに格納されているバイト数 */
    size_t      capacity;   /** dataのバイト数 */
} XMessageBuffer;


/** バッファを初期化します。
 *
 *  @pre
 *  + (buf != NULL) && (size > sizeof(XMessageHeader))
 *
 *  @attention
 *  XMessageBufferの使用中はbufが指す領域を破棄しないこと。
 */
static inline void
xmsgbuf_init(XMessageBuffer* self, void* buf, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(buf);
    X_ASSERT(size > sizeof(XMessageHeader));

    self->data = buf;
    self->first = self->last = self->size = 0;
    self->capacity = size;
}


/** 要素を格納するバッファを返します。
 */
static inline uint8_t*
xmsgbuf_data(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return self->data;
}


/** バッファを空にします。
 */
static inline void
xmsgbuf_clear(XMessageBuffer* self)
{
    X_ASSERT(self);
    self->first = self->last = self->size = 0;
}


/** バッファに格納されているバイト数を返します。
 */
static inline size_t
xmsgbuf_size(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return self->size;
}


/** バッファが空かどうかを返します。
 */
static inline bool
xmsgbuf_empty(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return self->size == 0;
}


/** バッファに格納できる最大バイト数を返します。
 */
static inline size_t
xmsgbuf_capacity(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return self->capacity;
}


/** バッファの空きバイト数を返します。
 */
static inline size_t
xmsgbuf_reserve(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return self->capacity - self->size;
}


/** バッファが満タンかどうかを返します。
 *
 *  xmsgbuf_reserve() <= sizeof(XMessageHeader)の時も、これ以上メッセージを格納できな
 *  い為、満タンと判定します。
 */
static inline bool
xmsgbuf_full(const XMessageBuffer* self)
{
    X_ASSERT(self);
    return xmsgbuf_reserve(self) <= sizeof(XMessageHeader);
}


/** 先頭メッセージのバイト数を返します。
 */
static inline size_t
xmsgbuf_msg_size(const XMessageBuffer* self)
{
    X_ASSERT(self);
    if (xmsgbuf_empty(self))
        return 0;

    XMessageHeader header;
    size_t rpos = self->first;
    int i;
    for (i = 0; i < (int)sizeof(XMessageHeader); i++) {
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
 *  + xmsgbuf_reserve() >= size + sizeof(XMessageHeader)
 */
static inline void
xmsgbuf_push(XMessageBuffer* self, const void* src, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(src);
    X_ASSERT(size > 0);
    X_ASSERT(xmsgbuf_reserve(self) >= size + sizeof(XMessageHeader));


    XMessageHeader header;
    header.size = size;
    self->size += sizeof(XMessageHeader) + size;

    size_t pos = self->last;
    int i;
    for (i = 0; i < (int)sizeof(XMessageHeader); i++)
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
 *  + dstが指す領域のバイト数 >= xmsgbuf_msg_size()
 */
static inline size_t
xmsgbuf_pull(XMessageBuffer* self, void* dst)
{
    X_ASSERT(self);
    X_ASSERT(dst);


    if (xmsgbuf_empty(self))
        return 0;

    XMessageHeader header;
    size_t pos = self->first;
    int i;
    for (i = 0; i < (int)sizeof(XMessageHeader); i++) {
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
    self->size -= header.size + sizeof(XMessageHeader);

    return header.size;
}


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // picox_container_xmessage_buffer_h_
