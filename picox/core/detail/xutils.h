/**
 *       @file  xutils.h
 *      @brief  低レベルなあれこれ雑多なユーティリティ集
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/14
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

#ifndef picox_core_xutils_h_
#define picox_core_xutils_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xutils
 *  @brief 低レベルのあれこれ雑多なユーティリティ集
 *
 *  アプリケーションコードであまり使うことはないと思いますが、デバイスドライバや
 *  通信プロトコル、バイナリファイルのパース等、細かい処理が必要な時に威力を発揮
 *  します。
 *
 *  数が多いので大変ですが、細かい処理が必要な時は、このモジュールのから探してみ
 *  ると、目的と合致するものが見つかるかもしれません。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


#if (X_CONF_BYTE_ORDER != X_BYTE_ORDER_LITTLE) && \
    (X_CONF_BYTE_ORDER != X_BYTE_ORDER_BIG)    && \
    (X_CONF_BYTE_ORDER != X_BYTE_ORDER_UNKNOWN)
    #error Invalid byte order
#endif


#define X_BYTE_ORDER X_CONF_BYTE_ORDER


/** @brief bit xを返します。
 */
#define X_BIT(x)       (1UL << (x))


/** @def   X_BIT64
 *  @brief X_BIT()の64ビット版です
 */
#ifndef X_COMPILER_NO_LONGLONG
    #define X_BIT64(x)     (1ULL << (x))
#endif


/** @brief n bit未満のbitを切り捨てるマスクを返します
 */
#define X_BIT_MASK(n) ((1UL << (n)) - 1)


/** @brief 型Tの変数として、x, yを交換します。
 */
#define X_SWAP(x, y, T) do { const T tmp = x; x = y; y = tmp; } while (0)


/** @brief a,bを比較し、小さい方を返します。
 */
#define X_MIN(a,b)     (((a)<(b))?(a):(b))


/** @brief a,bを比較し、大きい方を返します。
 */
#define X_MAX(a,b)     (((a)>(b))?(a):(b))


/** @brief xをa, bの範囲内に収めます。
 *
 *  @param a 下限
 *  @param b 上限
 *  @return
 *  xがa以上、b以下の時は、xがそのまま返ります。a未満のときはa, bより大きいとき
 *  はbが返ります。
 */
#define X_CONSTRAIN(x, a, b)    (((x) < (a)) ? (a) : ((b) < (x)) ? (b) : (x))


/** @brief cond == trueの時、break文を実行します。
 */
#define X_BREAK_IF(cond)   if(cond) break


/** @brief cond == trueの時、continue文を実行します。
 */
#define X_CONTINUE_IF(cond)   if(cond) continue


/** @brief cond == trueの時、式exprを実行します。
 */
#define X_EXPR_IF(cond, expr)   if(cond) expr


/** @brief cond == trueの時、labelへジャンプするgoto文を実行します。
 */
#define X_GOTO_IF(cond, label)   if(cond) goto label


/** @brief cond == trueの時、x に vを代入します。
 */
#define X_ASSIGN_IF(cond, x, v)   if(cond) x = (v)


/** @brief cond == trueの時、xにvを代入してlabelにジャンプします
 */
#define X_ASSIGN_AND_GOTO_IF(cond, x, v, label) if(cond) { (x) = (v); goto label; }


/** @brief ポインタ変数xがNULLでなければvを代入します
 */
#define X_ASSIGN_NOT_NULL(x, v)  if (x) *(x) = (v)


/** @brief cond == trueの時、return文を実行します。
 */
#define X_RETURN_IF(cond)   if(cond) return


/** @brief cond == trueの時、xを返すreturn文を実行します。
 */
#define X_RETURN_VALUE_IF(cond, x)   if(cond) return x


/** @brief コンパイラによる未使用変数の警告を抑制するマクロです。
 */
#define X_UNUSED(x)    (void)(x)


/** @brief 無限ループを表現するマクロです。
 */
#define X_FOREVER()    for (;;)


/** @brief あまりの切り上げを行う整数除算です
 */
#define X_DIV_ROUNDUP(dividend, divisor)  (((dividend) + (divisor) - 1) / (divisor))


/** @brief xをmの倍数に切り上げた値を返します。
 */
#define X_ROUNDUP_MULTIPLE(x, m)    (((m) == 0) ? (x) : (((uint32_t)(x) + (m) - 1) / (m)) * (m))


/** @brief X_ROUNDUP_MULTIPLE()の関数版です。
 */
uint32_t x_roundup_multiple(uint32_t x, uint32_t m);


/** @brief X_ROUNDUP_MULTIPLE()のポインタ版です。
 */
#define X_ROUNDUP_MULTIPLE_PTR(x, m)    (((m) == 0) ? (void*)(x) : (void*)(ptrdiff_t)((((uintptr_t)(x) + (m) - 1) / (m)) * (m)))


/** @brief X_ROUNDUP_MULTIPLE_PTR()の関数版です。
 */
void* x_roundup_multiple_ptr(const void* x, uint32_t m);


/** @brief xをmの倍数に切り下げた値を返します。
 */
#define X_ROUNDDOWN_MULTIPLE(x, m)   (((m) == 0) ? (x) : ((uint32_t)(x) - ((x) % (m))))


/** @brief X_ROUNDDOWN_MULTIPLE()の関数版です。
 */
uint32_t x_rounddown_multiple(uint32_t x, uint32_t m);


/** @brief xをmの倍数に切り下げた値を返します。
 */
#define X_ROUNDDOWN_MULTIPLE_PTR(x, m)   (((m) == 0) ? (void*)(x) : (void*)(ptrdiff_t)((uintptr_t)(x) - (((uintptr_t)(x)) % (m))))


/** @brief X_ROUNDDOWN_MULTIPLE()の関数版です。
 */
void* x_rounddown_multiple_ptr(const void* x, uintptr_t m);


/// @cond IGNORE
#define X_ROUNDUP_POWER_OF_TWO_5(x) (((x) | (x) >> 16) + 1)
#define X_ROUNDUP_POWER_OF_TWO_4(x) X_ROUNDUP_POWER_OF_TWO_5((x) | ((x) >> 8))
#define X_ROUNDUP_POWER_OF_TWO_3(x) X_ROUNDUP_POWER_OF_TWO_4((x) | ((x) >> 4))
#define X_ROUNDUP_POWER_OF_TWO_2(x) X_ROUNDUP_POWER_OF_TWO_3((x) | ((x) >> 2))
#define X_ROUNDUP_POWER_OF_TWO_1(x) X_ROUNDUP_POWER_OF_TWO_2((x) | ((x) >> 1))
/// @endcond IGNORE


/** @brief xを最も近い2のべき乗に切り上げた値を返します。
 */
#define X_ROUNDUP_POWER_OF_TWO(x)   X_ROUNDUP_POWER_OF_TWO_1((uint32_t)(x) - 1)


/** @brief X_ROUNDUP_POWER_OF_TWO()の関数版です。
 */
uint32_t x_roundup_power_of_two(uint32_t x);


/** @brief X_ROUNDUP_POWER_OF_TWO()のポインタ版です。
 */
#define X_ROUNDUP_POWER_OF_TWO_PTR(x)   ((void*)(ptrdiff_t)X_ROUNDUP_POWER_OF_TWO_1((uintptr_t)(x) - 1))


/** @brief X_ROUNDUP_POWER_OF_TWO_PTR()の関数版です。
 */
void* x_roundup_power_of_two_ptr(const void* x);


/// @cond IGNORE
#define X_ROUNDDOWN_POWER_OF_TWO_5(x) ((x) - ((x) >> 1))
#define X_ROUNDDOWN_POWER_OF_TWO_4(x) X_ROUNDDOWN_POWER_OF_TWO_5((x) | ((x) >> 16))
#define X_ROUNDDOWN_POWER_OF_TWO_3(x) X_ROUNDDOWN_POWER_OF_TWO_4((x) | ((x) >> 8))
#define X_ROUNDDOWN_POWER_OF_TWO_2(x) X_ROUNDDOWN_POWER_OF_TWO_3((x) | ((x) >> 4))
#define X_ROUNDDOWN_POWER_OF_TWO_1(x) X_ROUNDDOWN_POWER_OF_TWO_2((x) | ((x) >> 2))
/// @endcond IGNORE


/** @brief xを最も近い2のべき乗に切り下げた値を返します。
 */
#define X_ROUNDDOWN_POWER_OF_TWO(x)   X_ROUNDDOWN_POWER_OF_TWO_1(((uint32_t)(x)) | (((uint32_t)(x)) >> 1))


/** @brief X_ROUNDDOWN_POWER_OF_TWO()の関数版です。
 */
uint32_t x_rounddown_power_of_two(uint32_t x);


/** @brief X_ROUNDDOWN_POWER_OF_TWO()のポインタ版です。
 */
#define X_ROUNDDOWN_POWER_OF_TWO_PTR(x)   ((void*)(ptrdiff_t)X_ROUNDDOWN_POWER_OF_TWO_1(((uintptr_t)(x)) | (((uintptr_t)(x)) >> 1)))


/** @brief X_ROUNDDOWN_POWER_OF_TWO_PTR()の関数版です。
 */
void* x_rounddown_power_of_two_ptr(const void* x);


/** @brief xがmの倍数かどうかをBool値で返します。
 */
#define X_IS_MULTIPLE(x, m)  (X_ROUNDUP_MULTIPLE(x, m) == (x))


/** @brief X_IS_MULTIPLE()の関数版です。
 */
bool x_is_multiple(uint32_t x, uint32_t m);


/** @brief X_IS_MULTIPLE()のポインタ版です。
 */
#define X_IS_MULTIPLE_PTR(x, m)  (X_ROUNDUP_MULTIPLE_PTR(x, m) == (x))


/** @brief X_IS_MULTIPLE_PTR()の関数版です。
 */
bool x_is_multiple_ptr(const void* x, uint32_t m);


/** @brief xが2のべき乗かどうかをBool値で返します。
 */
#define X_IS_POWER_OF_TWO(x)   (((x) & -(x)) == (x))


/** @brief X_IS_POWER_OF_TWO()の関数版です。
 */
bool x_is_power_of_two(uint32_t x);


/** @brief X_IS_POWER_OF_TWO()のポインタ版です。
 */
#define X_IS_POWER_OF_TWO_PTR(x)   ((((intptr_t)(x)) & -((intptr_t)(x))) == ((intptr_t)(x)))


/** @brief X_IS_POWER_OF_TWO_PTR()の関数版です。
 */
bool x_is_power_of_two_ptr(const void* x);


/** @brief xをアライメントの倍数に切り上げた値を返します。
 *
 *  アライメントは2のべき乗であることを前提とします。
 */
#define X_ROUNDUP_ALIGNMENT(x, a) ((((uint32_t)(x)) + (a) - 1) & ((uint32_t)0 - (a)))


/** @brief X_ROUNDUP_ALIGNMENT()の関数版です。
 */
uint32_t x_roundup_alignment(uint32_t x, uint32_t a);


/** @brief X_ROUNDUP_ALIGNMENT()のポインタ版です。
 */
#define X_ROUNDUP_ALIGNMENT_PTR(x, a) ((void*)(ptrdiff_t)((((uintptr_t)(x)) + (a) - 1) & ((uintptr_t)0 - (a))))


/** @brief X_ROUNDUP_ALIGNMENT_PTR()の関数版です。
 */
void* x_roundup_alignment_ptr(const void* x, size_t a);


/** @brief xをアライメントの倍数に切り下げた値を返します。
 *
 *  アライメントは2のべき乗であることを前提とします。
 */
#define X_ROUNDDOWN_ALIGNMENT(x, a) (X_ROUNDUP_ALIGNMENT((x) - (a) + 1, a))


/** @brief X_ROUNDDOWN_ALIGNMENT()の関数版です。
 */
uint32_t x_rounddown_alignment(uint32_t x, uint32_t a);


/** @brief X_ROUNDDOWN_ALIGNMENT()のポインタ版です。
 */
#define X_ROUNDDOWN_ALIGNMENT_PTR(x, a) (X_ROUNDUP_ALIGNMENT_PTR(((uintptr_t)(x)) - (a) + 1, a))


/** @brief X_ROUNDDOWN_ALIGNMENT_PTR()の関数版です。
 */
void* x_rounddown_alignment_ptr(const void* x, uint32_t a);


/** @brief xが1または2のべき乗かどうかをBool値で返します。
 */
#define X_IS_ALIGNMENT(x)   (((uint32_t)(x) > 0) && (((uint32_t)(x) & ((uint32_t)(x) - 1)) == 0))


/** @brief X_IS_ALIGNMENT()の関数版です。
 */
bool x_is_alignment(uint32_t x);


/** @brief xがアライメントの倍数かどうかをBool値で返します。
 */
#define X_IS_ALIGNED(x, a)  (X_ROUNDUP_ALIGNMENT_PTR((x), (a)) == (x))


/** @brief ptrが指すアドレスがalignmentの倍数かどうかをBool値で返します。
 */
bool x_is_aligned(const void* ptr, size_t alignment);


/** @brief xの上位16bitの値を返します。
 */
#define X_HIGH_WORD(x) ((uint16_t)((x) >> 16))


/** @brief xの下位16bitの値を返します。
 */
#define X_LOW_WORD(x) ((uint16_t)(x))


/** @brief xの上位8bitの値を返します。
 */
#define X_HIGH_BYTE(x) ((uint8_t)((x) >> 8))


/** @brief xの下位8bitの値を返します。
 */
#define X_LOW_BYTE(x) ((uint8_t)(x))


/** @brief xの上位4bitの値を返します。
 */
#define X_HIGH_NIBBLE(x) (((uint8_t)(x)) >> 4)


/** @brief xの下位4bitの値を返します。
 */
#define X_LOW_NIBBLE(x) (((uint8_t)(x)) & 0x0f)


/** @brief xの下位8bitを逆転した値を返します。
 */
#define X_REVERSE_BITS8(x)  (((x) >> 7) & 0x01) | (((x) >> 5) & 0x02) | \
                            (((x) >> 3) & 0x04) | (((x) >> 1) & 0x08) | \
                            (((x) << 7) & 0x80) | (((x) << 5) & 0x40) | \
                            (((x) << 3) & 0x20) | (((x) << 1) & 0x10)


/** @brief X_REVERSE_BITS8()の関数版です。
 */
uint8_t x_reverse_bits8(uint8_t x);

/** @brief xの下位16bitを逆転した値を返します。
 */
#define X_REVERSE_BITS16(x) (((uint16_t)(X_REVERSE_BITS8(X_HIGH_BYTE(x)))) | \
                            (((uint16_t) X_REVERSE_BITS8(X_LOW_BYTE(x)))  << 8))


/** @brief X_REVERSE_BITS16()の関数版です。
 */
uint16_t x_reverse_bits16(uint16_t x);


/** @brief xの下位32bitを逆転した値を返します。
 */
#define X_REVERSE_BITS32(x) (((uint32_t)(X_REVERSE_BITS16(X_HIGH_WORD(x)))) | \
                            (((uint32_t)(X_REVERSE_BITS16(X_LOW_WORD(x)))) << 16))


/** @brief X_REVERSE_BITS32()の関数版です。
 */
uint32_t x_reverse_bits32(register uint32_t x);


/** @brief xの下位2バイトのバイトオーダーを逆転した値を返します。
 */
#define X_REVERSE_ENDIAN16(x)  ((((uint16_t)(x)) << 8) | (((uint16_t)(x) >> 8) & 0x00ff))


/** @brief X_REVERSE_ENDIAN16()の関数版です。
 */
uint16_t x_reverse_endian16(register uint16_t x);


/** @brief xの下位4バイトのバイトオーダーを逆転した値を返します。
 */
#define X_REVERSE_ENDIAN32(x) (((((uint32_t)(x)) << 24)      | \
                                (((x) <<  8) & 0x00ff0000)   | \
                                (((x) >>  8) & 0x0000ff00)   | \
                                (((x) >> 24) & 0x000000ff)))


/** @brief X_REVERSE_ENDIAN32()の関数版です。
 */
uint32_t x_reverse_endian32(uint32_t x);


/** @brief xの下位8bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS8(x)  (((uint8_t)(x)) & 0x55)


/** @brief xの下位16bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS16(x) (((uint16_t)(x)) & 0x5555)


/** @brief xの下位32bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS32(x) (((uint32_t)(x)) & 0x55555555)


/** @brief xの下位8bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS8(x)  (((uint8_t)(x)) & 0xaa)


/** @brief xの下位16bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS16(x) (((uint16_t)(x)) & 0xaaaa)


/** @brief xの下位32bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS32(x) (((uint32_t)(x)) & 0xaaaaaaaa)


/** @brief xの下位8bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS8(x)  ((X_ODD_BITS8(x) << 1) | ((X_EVEN_BITS8(x)) >> 1))


/** @brief xの下位16bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS16(x) ((X_ODD_BITS16(x) << 1) | ((X_EVEN_BITS16(x)) >> 1))


/** @brief xの下位32bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS32(x) ((X_ODD_BITS32(x) << 1) | ((X_EVEN_BITS32(x)) >> 1))


/** @brief ポインタから符号なし1バイトを取得して返します。
 */
#define X_LOAD_U8(ptr)  (*(uint8_t*)(ptr))


/** @brief ポインタから符号なし2Byteをリトルエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U16_LIT(ptr) ((uint16_t)(((uint16_t)*((uint8_t*)(ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr)))


/** @brief ポインタから符号なし4Byteをリトルエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U32_LIT(ptr)                                             \
            ((uint32_t)((((uint32_t)*((uint8_t*)(ptr) + 3)) << 24) |    \
                        (((uint32_t)*((uint8_t*)(ptr) + 2)) << 16) |    \
                        (((uint16_t)*((uint8_t*)(ptr) + 1)) <<  8) |    \
                         *(uint8_t*)(ptr)))


/** @brief ポインタから符号なし2Byteをビッグエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U16_BIG(ptr) (uint16_t)(((uint16_t)(*((uint8_t*)(ptr)))<<8)|(uint16_t)*((uint8_t*)(ptr) + 1))


/** @brief ポインタから符号なし4Byteをビッグエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U32_BIG(ptr)                                               \
            ((uint32_t)((((uint32_t)*(((uint8_t*)(ptr)) + 0)) << 24) |    \
                        (((uint32_t)*(((uint8_t*)(ptr)) + 1)) << 16) |    \
                        (((uint16_t)*(((uint8_t*)(ptr)) + 2)) <<  8) |    \
                         *(((uint8_t*)(ptr)) + 3)))


/** @brief 符号なし1Byteをポインタ参照先にセットします。
 */
#define X_STORE_U8(ptr, val)    (*(uint8_t*)(ptr)=(uint8_t)(val))


/** @brief 符号なし2Byteをポインタ参照先にホスト形式からリトルエンディアンでセットします。
 */
#define X_STORE_U16_LIT(ptr, val)                                   \
            (*(uint8_t*)(ptr)=(uint8_t)(val),                       \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8))     \


/** @brief 符号なし4Byteをポインタ参照先にホスト形式からリトルエンディアンでセットします。
 */
#define X_STORE_U32_LIT(ptr, val)                                   \
            (*(uint8_t*)(ptr)=(uint8_t)(val),                       \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8),     \
            *((uint8_t*)(ptr)+2)=(uint8_t)((uint32_t)(val)>>16),    \
            *((uint8_t*)(ptr)+3)=(uint8_t)((uint32_t)(val)>>24))


/** @brief 符号なし2Byteをポインタ参照先にホスト形式からビッグエンディアンでセットします。
 */
#define X_STORE_U16_BIG(ptr, val)                                   \
            (*((uint8_t*)(ptr)+1)=(uint8_t)(val),                   \
            *((uint8_t*)(ptr))=(uint8_t)((uint16_t)(val)>>8))       \


/** @brief 符号なし4Byteをポインタ参照先にホスト形式からビッグエンディアンでセットします。
 */
#define X_STORE_U32_BIG(ptr, val)                                   \
            (*((uint8_t*)(ptr)+3)=(uint8_t)(val),                   \
            *((uint8_t*)(ptr)+2)=(uint8_t)((uint16_t)(val)>>8),     \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint32_t)(val)>>16),    \
            *((uint8_t*)(ptr)+0)=(uint8_t)((uint32_t)(val)>>24))


/** @brief void*引数end, beginのバイト単位のアドレス差を返します。
 */
ptrdiff_t x_distance_ptr(const void* begin, const void* end);


/** @brief (begin <= x) && (x < end)を判定します。
 */
bool x_is_within(int32_t x, int32_t begin, int32_t end);


/** @brief (begin <= x) && (x < end)を判定します。
 */
bool x_is_uwithin(uint32_t x, uint32_t begin, uint32_t end);


/** @brief (begin <= x) && (x < end)を判定します。
 */
bool x_is_within_uptr(uintptr_t x, uintptr_t begin, uintptr_t end);


/** @brief ptrが指すアドレスがbegin とendの範囲内かどうかをBool値で返します。
 */
bool x_is_within_ptr(const void* ptr, const void* begin, const void* end);


/** @brief 8bit符号なし整数の下位から最も近くにセットされたビット位置を返します。
 *
 *  1つ以上のビットがセットされていることを前提としています。
 */
int x_find_lsb_pos8(uint8_t x);


/** @brief x_find_lsb8()の16bit版です
 */
int x_find_lsb_pos16(uint16_t x);


/** @brief x_find_lsb8()の32bit版です
 */
int x_find_lsb_pos32(uint32_t x);


/** @brief 8bit符号なし整数の下位から最も近くにセットされたビットを返します。
 *
 *  1つ以上のビットがセットされていることを前提としています。
 */
uint8_t  x_find_lsb8(uint8_t x);


/** @brief x_find_lsb8()の16bit版です
 */
uint16_t x_find_lsb16(uint16_t x);


/** @brief x_find_lsb8()の32bit版です
 */
uint32_t x_find_lsb32(uint32_t x);


/** @brief 8bit符号なし整数の上位から最も近くにセットされたビット位置を返します。
 *
 *  1つ以上のビットがセットされていることを前提としています。
 */
int x_find_msb_pos8(uint8_t x);


/** @brief x_find_msb8()の16bit版です
 */
int x_find_msb_pos16(uint16_t x);


/** @brief x_find_msb8()の32bit版です
 */
int x_find_msb_pos32(uint32_t x);


/** @brief 8bit符号なし整数の上位から最も近くにセットされたビットを返します。
 *
 *  1つ以上のビットがセットされていることを前提としています。
 */
uint8_t  x_find_msb8(uint8_t x);


/** @brief x_find_msb8()の16bit版です
 */
uint16_t x_find_msb16(uint16_t x);


/** @brief x_find_msb8()の32bit版です
 */
uint32_t x_find_msb32(uint32_t x);


int x_count_bits8(uint8_t x);


/** @brief x_count_bits8()の16bit版です
 */
int x_count_bits16(uint16_t x);


/** @brief x_count_bits8()の32bit版です
 */
int x_count_bits32(uint32_t x);


/** @brief xが指す先頭2バイトを逆転します
 *
 *  |A|B| -> |B|A|
 */
void x_reverse_2byte(void* x);


/** @brief xが指す先頭4バイトを逆転します
 *
 *  |A|B|C|D| -> |D|C|B|A|
 */
void x_reverse_4byte(void* x);


/** @brief 数値をある範囲から別の範囲に変換します
 *
 *  + http://www.musashinodenpa.com/arduino/ref/index.php?f=0&pos=2719
 */
int32_t x_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);


/** @brief x_map()の符号なし版です。
 */
uint32_t x_umap(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);


/** @brief バイトオーダーがビッグエンディアンかどうかを返します。
 */
bool x_is_big_endian(void);


/** @brief バイトオーダーがリトルエンディアンかどうかを返します。
 */
bool x_is_little_endian(void);


/** @brief 2Byteビッグエンディアンデータをホストのエンディアンにして返します。
 */
uint16_t x_big_to_host16(uint16_t x);


/** @brief 4Byteビッグエンディアンデータをホストのエンディアンにして返します。
 */
uint32_t x_big_to_host32(uint32_t x);


/** @brief 2Byteリトルエンディアンデータをホストのエンディアンにして返します。
 */
uint16_t x_little_to_host16(uint16_t x);


/** @brief 4Byteリトルエンディアンデータをホストのエンディアンにして返します。
 */
uint32_t x_little_to_host32(uint32_t x);


/** @brief 2Byteホストエンディアンデータをビッグエンディアンにして返します。
 */
uint16_t x_host_to_big16(uint16_t x);


/** @brief 4Byteホストエンディアンデータをビッグエンディアンにして返します。
 */
uint32_t x_host_to_big32(uint32_t x);


/** @brief 2Byteホストエンディアンデータをリトルエンディアンにして返します。
 */
uint16_t x_host_to_little16(uint16_t x);


/** @brief 4Byteホストエンディアンデータをリトルエンディアンにして返します。
 */
uint32_t x_host_to_little32(uint32_t x);


#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xutils
 *  @} end of addtogroup core
 */


#endif // picox_core_xutils_h_
