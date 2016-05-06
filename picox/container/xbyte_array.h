/**
 *       @file  xbyte_array.h
 *      @brief  可変長バイト配列
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/22
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

#ifndef picox_container_xbyte_array_h_
#define picox_container_xbyte_array_h_


#include <picox/core/xcore.h>


/** @addtogroup container
 *  @{
 *  @addtogroup xbyte_array
 *  @brief 可変長バイト配列モジュール
 *
 *  データはメモリ上に連続して配置されることを保証しているので、データポインタは
 *  、配列を操作するAPIに直接渡すことができます。
 *
 *  型がuint8_t固定なのでstd::vector<>に比べると使い勝手は数段劣りますが、それで
 *  も十分便利です。パフォーマンスを多少犠牲にしてバイトデータとして扱えば、
 *  uint8_t以外の型を格納することも可能です。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief 可変長バイト配列管理構造体
 */
typedef struct
{
/** @privatesection */
    uint8_t*    m_data;
    size_t      m_size;
    size_t      m_capacity;
    bool        m_is_heapdata;
} XByteArray;


/** @brief バッファを初期化します
 *
 *  @param buffer 可変長バイト配列に使用するメモリ領域
 *  @param size   バッファサイズ
 *
 *  @details
 *  buffer == NULLの場合は、ヒープからsizeバイト分のメモリ確保を行います。
 *  buffer != NULLの場合は、バッファの自動伸長は行われないことに注意してください
 *  。
 */
static inline bool
xbarray_init(XByteArray* self, void* buffer, size_t size)
{
    X_ASSERT(self);

    memset(self, 0, sizeof(*self));
    if (! buffer)
    {
        buffer = (uint8_t*)x_malloc(size);
        if (! buffer)
            return false;
        self->m_is_heapdata = true;
    }
    self->m_data = buffer;
    self->m_capacity = size;

    return true;
}


/** @brief バッファを破棄します
 */
static inline void
xbarray_deinit(XByteArray* self)
{
    X_ASSERT(self);
    if (self->m_is_heapdata)
    {
        x_free(self->m_data);
        self->m_is_heapdata = false;
    }
    self->m_data = NULL;
    self->m_size = 0;
    self->m_capacity = 0;
}


/** @brief バッファ先頭を指すコンストポインタを返します
 */
static inline const uint8_t*
xbarray_const_data(const XByteArray* self)
{
    X_ASSERT(self);
    return self->m_data;
}


/** @brief バッファ先頭を指すポインタを返します
 */
static inline uint8_t*
xbarray_data(XByteArray* self)
{
    return (uint8_t*)xbarray_const_data(self);
}


/** @brief バッファ先頭からnバイト後方を指すコンストポインタを返します
 */
static inline const uint8_t*
xbarray_const_at(const XByteArray* self, size_t index)
{
    X_ASSERT(self);
    X_ASSERT(self->m_size > index);
    return self->m_data + index;
}


/** @brief バッファ先頭からnバイト後方を指すポインタを返します
 */
static inline uint8_t*
xbarray_at(XByteArray* self, size_t index)
{
    return (uint8_t*)xbarray_const_at(self, index);
}


/** @brief バッファに格納されたバイト数を返します
 */
static inline size_t
xbarray_size(const XByteArray* self)
{
    X_ASSERT(self);
    return self->m_size;
}


/** @brief バッファに割り当てられたバイト数を返します
 */
static inline size_t
xbarray_capacity(const XByteArray* self)
{
    X_ASSERT(self);
    return self->m_capacity;
}


/** @brief バッファが空かどうかを返します
 */
static inline bool
xbarray_empty(const XByteArray* self)
{
    X_ASSERT(self);
    return ! self->m_size;
}


/** @brief バッファが拡張なしで格納できる上限に達しているかどうかを返します
 */
static inline bool
xbarray_full(const XByteArray* self)
{
    X_ASSERT(self);
    return self->m_size == self->m_capacity;
}


/** @brief バッファが拡張なしで格納できる空きバイト数を返します
 */
static inline size_t
xbarray_space(const XByteArray* self)
{
    X_ASSERT(self);
    return self->m_capacity - self->m_size;
}


/** @brief バッファの要素数を0にします
 */
static inline void
xbarray_clear(XByteArray* self)
{
    X_ASSERT(self);
    self->m_size = 0;
}


/** @brief バッファをvalueで埋めます
 */
static inline void
xbarray_fill(XByteArray* self, uint8_t value)
{
    X_ASSERT(self);
    memset(self->m_data, value, self->m_size);
}


/** @brief バッファにsizeバイト以上の容量を確保します
 *
 *  @retval true  メモリ確保成功
 *  @retval false メモリ確保失敗
 *
 *  @details
 *  メモリ確保失敗時は、元のバッファはそのまま保持されます。
 *
 *  要素追加時のメモリ伸長をX_ASSERT()で検査していますが、失敗時にプログラムを停
 *  止させたくない場合は、前もってメモリを予約することで成功を保証きます。
 *  また、必要な容量が予めある程度わかっているときは、先に予約をしておくことで、
 *  再配置のオーバーヘッドを最小にすることができます。
 */
static inline bool
xbarray_reserve(XByteArray* self, size_t size)
{
    X_ASSERT(self);
    if (self->m_capacity >= size)
        return true;

    if (! self->m_is_heapdata)
        return false;

    uint8_t* const buf = (uint8_t*)x_realloc2(self->m_data,
                                              self->m_size,
                                              size);
    if (! buf)
        return false;

    self->m_data = buf;
    self->m_capacity = size;

    return true;
}


/** @brief sizeバイトを格納する空き容量がない場合、バッファを拡張します
 *
 *  @retval true  メモリ確保成功
 *  @retval false メモリ確保失敗
 *
 *  @details
 *  メモリ確保失敗時は、元のバッファはそのまま保持されます
 */
static inline bool
xbarray_make_space_if(XByteArray* self, size_t size)
{
    const size_t space = self->m_capacity - self->m_size;
    if (space >= size)
        return true;

    const size_t new_capcity = size > space + self->m_capacity ?
                               size :
                               self->m_capacity * 2;
    return xbarray_reserve(self, new_capcity);
}


/** @brief X_ASSERT()による検査を行うxbarray_make_space_if()です
 */
static inline void
xbarray_strict_make_space_if(XByteArray* self, size_t size)
{
    const bool ok = xbarray_make_space_if(self, size);
    X_ASSERT(ok);
    X_UNUSED(ok);
}


/** @brief バッファ末尾にsrcからsizeバイトを追加します
 */
static inline void
xbarray_push_back_n(XByteArray* self, const void* src, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(src);

    xbarray_strict_make_space_if(self, size);
    memcpy(self->m_data + self->m_size, src, size);
    self->m_size += size;
}


/** @brief バッファ末尾にvalueを追加します
 */
static inline void
xbarray_push_back(XByteArray* self, uint8_t value)
{
    X_ASSERT(self);

    xbarray_strict_make_space_if(self, sizeof(value));
    *(self->m_data + self->m_size) = value;
    self->m_size += 1;
}


/** @brief バッファ末尾からsizeバイトを除去します
 *
 *  dstがNULLでなければ除去されるデータがdstにコピーされます
 */
static inline void
xbarray_pop_back_n(XByteArray* self, void* dst, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(self->m_size >= size);

    self->m_size -= size;
    if (dst)
        memcpy(dst, self->m_data + self->m_size, size);
}


/** @brief バッファ末尾から1バイトを除去して返します
 */
static inline uint8_t
xbarray_pop_back(XByteArray* self)
{
    X_ASSERT(self);
    self->m_size -= 1;
    const uint8_t value = *(self->m_data + self->m_size);

    return value;
}


/** @brief バッファの容量を有効バイト数まで切り詰めます
 *
 *  メモリ再配置によるオーバーヘッドを減らすために、メモリ伸長時はある程度多めに
 *  確保を行います。
 *  この関数が成功すると、xbarray_size() == xbarray_capcity()になります。
 */
static inline bool
xbarray_shrink_to_fit(XByteArray* self)
{
    X_ASSERT(self);

    uint8_t* const buf = (uint8_t*)x_realloc2(self->m_data,
                                              self->m_capacity,
                                              self->m_size);
    if (! buf)
        return false;

    self->m_data = buf;
    self->m_capacity = self->m_size;

    return true;
}


/** @brief バッファ先頭からn番目にsrcからsizeバイトを挿入します
 */
static inline void
xbarray_insert_n(XByteArray* self, size_t index, const void* src, size_t size)
{
    X_ASSERT(self);

    if (index == self->m_size)
    {
        xbarray_push_back_n(self, src, size);
        return;
    }

    X_ASSERT(src);
    X_ASSERT(index <= self->m_size);

    xbarray_strict_make_space_if(self, size);
    memmove(self->m_data + index + size,
            self->m_data + index,
            self->m_size - index);
    memcpy(self->m_data + index, src, size);
    self->m_size += size;
}


/** @brief バッファ先頭からn番目にvalueを挿入します
 */
static inline void
xbarray_insert(XByteArray* self, size_t index, uint8_t value)
{
    xbarray_insert_n(self, index, &value, sizeof(value));
}


/** @brief バッファ先頭からn番目 ~ n + sizeの範囲を除去します
 */
static inline void
xbarray_erase_n(XByteArray* self, size_t index, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(index <= self->m_size);

    memmove(self->m_data + index,
            self->m_data + index + size,
            self->m_size - (index + size));
    self->m_size -= size;
}


/** @brief バッファ先頭からn番目を除去します
 */
static inline void
xbarray_erase(XByteArray* self, size_t index)
{
    xbarray_erase_n(self, index, sizeof(uint8_t));
}


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xbyte_array
 *  @} end of addtogroup container
 */


#endif /* picox_container_xbyte_array_h_ */
