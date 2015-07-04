/**
 *       @file  utils.h
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

#ifndef picox_core_utils_h_
#define picox_core_utils_h_


#ifdef __cplusplus
extern "C" {
#endif


#define X_BYTE_ORDER_LITTLE    (0)
#define X_BYTE_ORDER_BIG       (1)
#define X_BYTE_ORDER_UNKNOWN   (2)


#ifdef X_CONF_BYTE_ORDER
    #define X_BYTE_ORDER    X_CONF_BYTE_ORDER
    #if (X_BYTE_ORDER != X_BYTE_ORDER_LITTLE) && (X_BYTE_ORDER != X_BYTE_ORDER_BIG)
        #error Invalid byte order
    #endif
#else
    #define X_BYTE_ORDER    X_BYTE_ORDER_UNKNOWN
#endif



/** 組込み型の最大アライメント型です。
 */
typedef union XMaxAlign
{
    char c; int i; long l; long long ll; void* p;
    float f; double d; long double ld; void(*fp)(void);
} XMaxAlign;


/** 最大サイズのアライメントでsizeバイト以上の領域を持つ変数nameを定義します。
 */
#define X_DEF_MAX_ALIGNED(name, size) XMaxAlign name[((size) + sizeof(XMaxAlign) - 1) / sizeof(XMaxAlign)]


/** 構造体や共用体メンバのsizeofを返します。
 */
#define X_SIZEOF_MEM(s, m) (sizeof(((s*)0)->m))


/** 構造体や共用体メンバの先頭からのオフセットを返します。
 */
#define X_OFFSET_OF(s, m)   ((uintptr_t)&(((s *)0)->m))


/** 型typeのアライメントを返します。
 */
#define X_ALIGN_OF(type)   X_OFFSET_OF(struct { char c; type member; }, member)


/** @def    X_CONTAINER_OF
 *  @brief  複合型のメンバを指すポインタから、複合型の先頭アドレスを取得します
 *
 *  @param  ptr    複合型typeのmemberを指すポインタ
 *  @param  type   memberをメンバに持つ複合型
 *  @param  member ptrが指す複合型のメンバ名
 *
 *  @code
 *  typedef struct Foo
 *   {
 *       int          a;
 *       int          b;
 *  } Foo;

 *  struct Foo foo;
 *  int* mem_ptr = &foo.b;
 *  Foo* foo_ptr = X_CONTAINER_OF(mem_ptr, struct Foo, b);
 *  assert(&foo == foo_ptr);
 *  @endcode
 *
 *  @note
 *  コンパイラ拡張が使用できる場合、ptrがmemberと同じ型のポインタであることを
 *  チェックすることができます。
 *
 *  通常は型チェックをすることができないので、ptrに間違えたポインタを指定してし
 *  まった場合、発見の難しい申告なバグの原因となりえるので注意してください。
 */
#if defined(X_HAS_TYPEOF) && defined(X_HAS_STATEMENTS_AND_DECLARATIONS_IN_EXPRESSIONS)
    #define X_CONTAINER_OF(ptr, type, member)                     \
            ({                                                    \
                const X_TYPEOF(((type*)0)->member)* mptr = (ptr); \
                (type*)((char*)mptr - X_OFFSET_OF(type,member) ); \
            })
#else
    #define X_CONTAINER_OF(ptr, type, member)                     \
            ((type*) ((char*)(ptr) - X_OFFSET_OF(type, member)))
#endif


/// @cond IGNORE
#define X_STATIC_ASSERT_CAT_(a, b)          a ## b
/// @endcond IGNORE


/** コンパイル時アサートを行います。
 */
#define X_STATIC_ASSERT(cond)  \
    enum { X_STATIC_ASSERT_CAT_(X_STATIC_ASSERTION_FAILED, __LINE__) = \
          sizeof( struct { int assertion_failed[(cond) ? 1: -1];})}


/** bit xを返します。
 */
#define X_BIT(x)       (1UL << (x))


/** 型Tの変数として、x, yを交換します。
 */
#define X_SWAP(x, y, T) do { T tmp = x; x = y; y = tmp; } while (0)


/** a,bを比較し、小さい方を返します。
 */
#define X_MIN(a,b)     (((a)<(b))?(a):(b))


/** a,bを比較し、大きい方を返します。
 */
#define X_MAX(a,b)     (((a)>(b))?(a):(b))


/** xをa, bの範囲内に収めます。
 *
 *  @param a 下限
 *  @param b 上限
 *  @return
 *  xがa以上、b以下の時は、xがそのまま返ります。a未満のときはa, bより大きいとき
 *  はbが返ります。
 */
#define X_CONSTRAIN(x, a, b)    (((x) < (a)) ? (a) : ((b) < (x)) ? (b) : (x))


/** cond == trueの時、break文を実行します。
 */
#define X_BREAK_IF(cond)   if(cond) break


/** cond == trueの時、式exprを実行します。
 */
#define X_EXPR_IF(cond, expr)   if(cond) expr


/** cond == trueの時、labelへジャンプするgoto文を実行します。
 */
#define X_GOTO_IF(cond, label)   if(cond) goto label


/** cond == trueの時、x に vを代入します。
 */
#define X_ASIGN_IF(cond, x, v)   if(cond) x = v


/** cond == trueの時、return文を実行します。
 */
#define X_RETURN_IF(cond)   if(cond) return


/** cond == trueの時、xを返すreturn文を実行します。
 */
#define X_RETURN_VALUE_IF(cond, x)   if(cond) return x


/** コンパイラによる未使用変数の警告を抑制するマクロです。
 */
#define X_UNUSED(x)    (void)(x)


/** 無限ループを表現するマクロです。
 */
#define X_FOREVER()    for (;;)


/** xビット目のビットを返します。
 */
#define X_BIT(x) (1UL << (x))


/** 一次元配列の要素数を返します。
 */
#define X_COUNT_OF(a)      (sizeof(a) / sizeof(*(a)))


/** 二次元配列の行要素数を返します。
 */
#define X_COUNT_OF_ROW(x) (sizeof(x) / sizeof(x[0]))


/** 二次元配列の列要素数を返します。
 */
#define X_COUNT_OF_COL(x) (sizeof(x[0]) / sizeof(x[0][0]))


/** 二次元配列の要素数を返します。
 */
#define X_COUNT_OF_2D(x)   (X_COUNT_OF_ROW(x) * X_COUNT_OF_COL(x))


/** xをmの倍数に切り上げた値を返します。
 */
#define X_ROUNDUP_MULTIPLE(x, m)    (((m) == 0) ? (x) : (((uint32_t)(x) + (m) - 1) / (m)) * (m))


/** X_ROUNDUP_MULTIPLE()の関数版です。
 */
static inline uint32_t x_roundup_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDUP_MULTIPLE(x, m);
}


/** X_ROUNDUP_MULTIPLE()のポインタ版です。
 */
#define X_ROUNDUP_MULTIPLE_PTR(x, m)    (((m) == 0) ? (void*)(x) : (void*)((((uintptr_t)(x) + (m) - 1) / (m)) * (m)))


/** X_ROUNDUP_MULTIPLE_PTR()の関数版です。
 */
static inline void* x_roundup_multiple_ptr(const void* x, uint32_t m)
{
    return (void*)X_ROUNDUP_MULTIPLE_PTR(x, m);
}


/** xをmの倍数に切り下げた値を返します。
 */
#define X_ROUNDDOWN_MULTIPLE(x, m)   (((m) == 0) ? (x) : ((uint32_t)(x) - ((x) % (m))))


/** X_ROUNDDOWN_MULTIPLE()の関数版です。
 */
static inline uint32_t x_rounddown_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDDOWN_MULTIPLE(x, m);
}


/** xをmの倍数に切り下げた値を返します。
 */
#define X_ROUNDDOWN_MULTIPLE_PTR(x, m)   (((m) == 0) ? (void*)(x) : (void*)((uintptr_t)(x) - (((uintptr_t)(x)) % (m))))


/** X_ROUNDDOWN_MULTIPLE()の関数版です。
 */
static inline void* x_rounddown_multiple_ptr(const void* x, uintptr_t m)
{
    return (void*)X_ROUNDDOWN_MULTIPLE_PTR(x, m);
}


/// @cond IGNORE
#define X_ROUNDUP_POWER_OF_TWO_5(x) (((x) | (x) >> 16) + 1)
#define X_ROUNDUP_POWER_OF_TWO_4(x) X_ROUNDUP_POWER_OF_TWO_5((x) | ((x) >> 8))
#define X_ROUNDUP_POWER_OF_TWO_3(x) X_ROUNDUP_POWER_OF_TWO_4((x) | ((x) >> 4))
#define X_ROUNDUP_POWER_OF_TWO_2(x) X_ROUNDUP_POWER_OF_TWO_3((x) | ((x) >> 2))
#define X_ROUNDUP_POWER_OF_TWO_1(x) X_ROUNDUP_POWER_OF_TWO_2((x) | ((x) >> 1))
/// @endcond IGNORE


/** xを最も近い2のべき乗に切り上げた値を返します。
 */
#define X_ROUNDUP_POWER_OF_TWO(x)   X_ROUNDUP_POWER_OF_TWO_1((uint32_t)(x) - 1)


/** X_ROUNDUP_POWER_OF_TWO()の関数版です。
 */
static inline uint32_t x_roundup_power_of_two(uint32_t x)
{
    return X_ROUNDUP_POWER_OF_TWO(x);
}


/** X_ROUNDUP_POWER_OF_TWO()のポインタ版です。
 */
#define X_ROUNDUP_POWER_OF_TWO_PTR(x)   ((void*)X_ROUNDUP_POWER_OF_TWO_1((uintptr_t)(x) - 1))


/** X_ROUNDUP_POWER_OF_TWO_PTR()の関数版です。
 */
static inline void* x_roundup_power_of_two_ptr(const void* x)
{
    return (void*)X_ROUNDUP_POWER_OF_TWO_PTR(x);
}


/// @cond IGNORE
#define X_ROUNDDOWN_POWER_OF_TWO_5(x) ((x) - ((x) >> 1))
#define X_ROUNDDOWN_POWER_OF_TWO_4(x) X_ROUNDDOWN_POWER_OF_TWO_5((x) | ((x) >> 16))
#define X_ROUNDDOWN_POWER_OF_TWO_3(x) X_ROUNDDOWN_POWER_OF_TWO_4((x) | ((x) >> 8))
#define X_ROUNDDOWN_POWER_OF_TWO_2(x) X_ROUNDDOWN_POWER_OF_TWO_3((x) | ((x) >> 4))
#define X_ROUNDDOWN_POWER_OF_TWO_1(x) X_ROUNDDOWN_POWER_OF_TWO_2((x) | ((x) >> 2))
/// @endcond IGNORE


/** xを最も近い2のべき乗に切り下げた値を返します。
 */
#define X_ROUNDDOWN_POWER_OF_TWO(x)   X_ROUNDDOWN_POWER_OF_TWO_1(((uint32_t)(x)) | (((uint32_t)(x)) >> 1))


/** X_ROUNDDOWN_POWER_OF_TWO()の関数版です。
 */
static inline uint32_t x_rounddown_power_of_two(uint32_t x)
{
    return X_ROUNDDOWN_POWER_OF_TWO(x);
}


/** X_ROUNDDOWN_POWER_OF_TWO()のポインタ版です。
 */
#define X_ROUNDDOWN_POWER_OF_TWO_PTR(x)   ((void*)X_ROUNDDOWN_POWER_OF_TWO_1(((uintptr_t)(x)) | (((uintptr_t)(x)) >> 1)))


/** X_ROUNDDOWN_POWER_OF_TWO_PTR()の関数版です。
 */
static inline void* x_rounddown_power_of_two_ptr(const void* x)
{
    return X_ROUNDDOWN_POWER_OF_TWO_PTR(x);
}


/** xがmの倍数かどうかをBool値で返します。
 */
#define X_IS_MULTIPLE(x, m)  (X_ROUNDUP_MULTIPLE(x, m) == (x))


/** X_IS_MULTIPLE()の関数版です。
 */
static inline bool x_is_multiple(uint32_t x, uint32_t m)
{
    return X_IS_MULTIPLE(x, m);
}


/** X_IS_MULTIPLE()のポインタ版です。
 */
#define X_IS_MULTIPLE_PTR(x, m)  (X_ROUNDUP_MULTIPLE_PTR(x, m) == (x))


/** X_IS_MULTIPLE_PTR()の関数版です。
 */
static inline bool x_is_multiple_ptr(const void* x, uint32_t m)
{
    return X_IS_MULTIPLE_PTR(x, m);
}


/** xが2のべき乗かどうかをBool値で返します。
 */
#define X_IS_POWER_OF_TWO(x)   (((x) & -(x)) == (x))


/** X_IS_POWER_OF_TWO()の関数版です。
 */
static inline bool x_is_power_of_two(uint32_t x)
{
    return X_IS_POWER_OF_TWO(x);
}


/** X_IS_POWER_OF_TWO()のポインタ版です。
 */
#define X_IS_POWER_OF_TWO_PTR(x)   ((((intptr_t)(x)) & -((intptr_t)(x))) == ((intptr_t)(x)))


/** X_IS_POWER_OF_TWO_PTR()の関数版です。
 */
static inline bool x_is_power_of_two_ptr(const void* x)
{
    return X_IS_POWER_OF_TWO_PTR(x);
}


/** xをアライメントの倍数に切り上げた値を返します。
 *
 *  @note
 *  アライメントは2のべき乗であることを前提とします。
 */
#define X_ROUNDUP_ALIGNMENT(x, a) ((((uint32_t)(x)) + (a) - 1) & ((uint32_t)0 - (a)))


/** X_ROUNDUP_ALIGNMENT()の関数版です。
 */
static inline uint32_t x_roundup_alignment(uint32_t x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDUP_ALIGNMENT(x, a);
}


/** X_ROUNDUP_ALIGNMENT()のポインタ版です。
 */
#define X_ROUNDUP_ALIGNMENT_PTR(x, a) ((void*)((((uintptr_t)(x)) + (a) - 1) & ((uintptr_t)0 - (a))))


/** X_ROUNDUP_ALIGNMENT_PTR()の関数版です。
 */
static inline void* x_roundup_alignment_ptr(const void* x, size_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDUP_ALIGNMENT_PTR(x, a);
}


/** xをアライメントの倍数に切り下げた値を返します。
 *
 *  @note
 *  アライメントは2のべき乗であることを前提とします。
 */
#define X_ROUNDDOWN_ALIGNMENT(x, a) (X_ROUNDUP_ALIGNMENT((x) - (a) + 1, a))


/** X_ROUNDDOWN_ALIGNMENT()の関数版です。
 */
static inline uint32_t x_rounddown_alignment(uint32_t x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDDOWN_ALIGNMENT(x, a);
}


/** X_ROUNDDOWN_ALIGNMENT()のポインタ版です。
 */
#define X_ROUNDDOWN_ALIGNMENT_PTR(x, a) (X_ROUNDUP_ALIGNMENT_PTR(((uintptr_t)(x)) - (a) + 1, a))


/** X_ROUNDDOWN_ALIGNMENT_PTR()の関数版です。
 */
static inline void* x_rounddown_alignment_ptr(const void* x, uint32_t a)
{
    X_ASSERT(X_IS_POWER_OF_TWO(a));
    return X_ROUNDDOWN_ALIGNMENT_PTR(x, a);
}


/** xが1または2のべき乗かどうかをBool値で返します。
 */
#define X_IS_ALIGNMENT(x)   (((uint32_t)(x) > 0) && (((uint32_t)(x) & ((uint32_t)(x) - 1)) == 0))


/** X_IS_ALIGNMENT()の関数版です。
 */
static inline bool x_is_alignment(uint32_t x)
{
    return X_IS_ALIGNMENT(x);
}


/** xがアライメントの倍数かどうかをBool値で返します。
 */
#define X_IS_ALIGNED(x, a)  (X_ROUNDUP_ALIGNMENT_PTR((x), (a)) == (x))


/** ptrが指すアドレスがalignmentの倍数かどうかをBool値で返します。
 */
static inline bool x_is_aligned(const void* ptr, size_t alignment)
{
    X_ASSERT(X_IS_POWER_OF_TWO(alignment));

    const bool ok = (X_ROUNDUP_ALIGNMENT_PTR(ptr, alignment) == ptr);
    return ok;
}


/** xの上位16bitの値を返します。
 */
#define X_HIGH_WORD(x) ((uint16_t)((x) >> 16))


/** xの下位16bitの値を返します。
 */
#define X_LOW_WORD(x) ((uint16_t)(x))


/** xの上位8bitの値を返します。
 */
#define X_HIGH_BYTE(x) ((uint8_t)((x) >> 8))


/** xの下位8bitの値を返します。
 */
#define X_LOW_BYTE(x) ((uint8_t)(x))


/** xの上位4bitの値を返します。
 */
#define X_HIGH_NIBBLE(x) (((uint8_t)(x)) >> 4)


/** xの下位4bitの値を返します。
 */
#define X_LOW_NIBBLE(x) (((uint8_t)(x)) & 0x0f)


/** xの下位8bitを逆転した値を返します。
 */
#define X_REVERSE_BITS8(x)  (((x) >> 7) & 0x01) | (((x) >> 5) & 0x02) | \
                            (((x) >> 3) & 0x04) | (((x) >> 1) & 0x08) | \
                            (((x) << 7) & 0x80) | (((x) << 5) & 0x40) | \
                            (((x) << 3) & 0x20) | (((x) << 1) & 0x10)


/// @cond IGNORE
#define X_DECLARE_BIT_REVERSE_TABLE        \
    const uint8_t bit_reverse_table[16] =   \
        {                                   \
            0,  /* 0000 -> 0000 */          \
            8,  /* 0001 -> 1000 */          \
            4,  /* 0010 -> 0100 */          \
            12, /* 0011 -> 1100 */          \
            2,  /* 0100 -> 0010 */          \
            10, /* 0101 -> 1010 */          \
            6,  /* 0110 -> 0110 */          \
            14, /* 0111 -> 1110 */          \
            1,  /* 1000 -> 0001 */          \
            9,  /* 1001 -> 1001 */          \
            5,  /* 1010 -> 0101 */          \
            13, /* 1011 -> 1101 */          \
            3,  /* 1100 -> 0011 */          \
            11, /* 1101 -> 1011 */          \
            7,  /* 1110 -> 0111 */          \
            15, /* 1111 -> 1111 */          \
        }
/// @endcond IGNORE


/** X_REVERSE_BITS8()の関数版です。
 */
static inline uint8_t x_reverse_bits8(uint8_t x)
{
#if 0
    x = (x & 0xF0) >> 4 | (x & 0x0F) << 4;
    x = (x & 0xCC) >> 2 | (x & 0x33) << 2;
    x = (x & 0xAA) >> 1 | (x & 0x55) << 1;
    return x;
#endif
    X_DECLARE_BIT_REVERSE_TABLE;
    const uint8_t a = bit_reverse_table[(x >> 4)];
    const uint8_t b = bit_reverse_table[(x & 0xf)] << 4;
    return a | b;
}

/** xの下位16bitを逆転した値を返します。
 */
#define X_REVERSE_BITS16(x) (((uint16_t)(X_REVERSE_BITS8(X_HIGH_BYTE(x)))) | \
                            (((uint16_t) X_REVERSE_BITS8(X_LOW_BYTE(x)))  << 8))


/** X_REVERSE_BITS16()の関数版です。
 */
static inline uint16_t x_reverse_bits16(uint16_t x)
{
#if 0
    x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
    x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
    x = ((x >> 4) & 0x0F0F) | ((x & 0x0F0F) << 4);
    x = ((x >> 8) & 0x00FF) | ((x & 0x00FF) << 8);
    return x;
#endif
    const uint16_t a = x_reverse_bits8(x >> 8);
    const uint16_t b = x_reverse_bits8(x) << 8;
    return a | b;
}


/** xの下位32bitを逆転した値を返します。
 */
#define X_REVERSE_BITS32(x) (((uint32_t)(X_REVERSE_BITS16(X_HIGH_WORD(x)))) | \
                            (((uint32_t)(X_REVERSE_BITS16(X_LOW_WORD(x)))) << 16))


/** X_REVERSE_BITS32()の関数版です。
 */
static inline uint32_t x_reverse_bits32(register uint32_t x)
{
#if 0
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return ((x >> 16) | (x << 16));
#endif
    const uint32_t a = x_reverse_bits16(x >> 16);
    const uint32_t b = ((uint32_t)x_reverse_bits16(x)) << 16;
    return a | b;
}


/** xの下位2バイトのバイトオーダーを逆転した値を返します。
 */
#define X_REVERSE_ENDIAN16(x)  ((((uint16_t)(x)) << 8) | (((uint16_t)(x) >> 8) & 0x00ff))


/** X_REVERSE_ENDIAN16()の関数版です。
 */
static inline uint16_t x_reverse_endian16(register uint16_t x)
{
    return X_REVERSE_ENDIAN16(x);
}


/** xの下位4バイトのバイトオーダーを逆転した値を返します。
 */
#define X_REVERSE_ENDIAN32(x) (((((uint32_t)(x)) << 24)      | \
                                (((x) <<  8) & 0x00ff0000)   | \
                                (((x) >>  8) & 0x0000ff00)   | \
                                (((x) >> 24) & 0x000000ff)))


/** X_REVERSE_ENDIAN32()の関数版です。
 */
static inline uint32_t x_reverse_endian32(uint32_t x)
{
    return X_REVERSE_ENDIAN32(x);
}


/** xの下位8bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS8(x)  (((uint8_t)(x)) & 0x55)


/** xの下位16bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS16(x) (((uint16_t)(x)) & 0x5555)


/** xの下位32bitを奇数ビットでマスクした値を返します。
 */
#define X_ODD_BITS32(x) (((uint32_t)(x)) & 0x55555555)


/** xの下位8bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS8(x)  (((uint8_t)(x)) & 0xaa)


/** xの下位16bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS16(x) (((uint16_t)(x)) & 0xaaaa)


/** xの下位32bitを偶数ビットでマスクした値を返します。
 */
#define X_EVEN_BITS32(x) (((uint32_t)(x)) & 0xaaaaaaaa)


/** xの下位8bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS8(x)  ((X_ODD_BITS8(x) << 1) | ((X_EVEN_BITS8(x)) >> 1))


/** xの下位16bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS16(x) ((X_ODD_BITS16(x) << 1) | ((X_EVEN_BITS16(x)) >> 1))


/** xの下位32bitの隣り合ったビットを交換した値を返します。
 */
#define X_SWAP_ADJACENT_BITS32(x) ((X_ODD_BITS32(x) << 1) | ((X_EVEN_BITS32(x)) >> 1))


/** ポインタから符号なし1バイトを取得して返します。
 */
#define X_LOAD_U8(ptr)  (*(uint8_t*)(ptr))


/** ポインタから符号なし2Byteをリトルエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U16_LIT(ptr) ((uint16_t)(((uint16_t)*((uint8_t*)(ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr)))


/** ポインタから符号なし4Byteをリトルエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U32_LIT(ptr)                                             \
            ((uint32_t)((((uint32_t)*((uint8_t*)(ptr) + 3)) << 24) |    \
                        (((uint32_t)*((uint8_t*)(ptr) + 2)) << 16) |    \
                        (((uint16_t)*((uint8_t*)(ptr) + 1)) <<  8) |    \
                         *(uint8_t*)(ptr)))


/** ポインタから符号なし2Byteをビッグエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U16_BIG(ptr) (uint16_t)(((uint16_t)(*((uint8_t*)(ptr)))<<8)|(uint16_t)*((uint8_t*)(ptr) + 1))


/** ポインタから符号なし4Byteをビッグエンディアンからホスト形式で取得して返します。
 */
#define X_LOAD_U32_BIG(ptr)                                               \
            ((uint32_t)((((uint32_t)*(((uint8_t*)(ptr)) + 0)) << 24) |    \
                        (((uint32_t)*(((uint8_t*)(ptr)) + 1)) << 16) |    \
                        (((uint16_t)*(((uint8_t*)(ptr)) + 2)) <<  8) |    \
                         *(((uint8_t*)(ptr)) + 3)))


/** 符号なし1Byteをポインタ参照先にセットします。
 */
#define X_STORE_U8(ptr, val)    (*(uint8_t*)(ptr)=(uint8_t)(val))


/** 符号なし2Byteをポインタ参照先にホスト形式からリトルエンディアンでセットします。
 */
#define X_STORE_U16_LIT(ptr, val)                                   \
            (*(uint8_t*)(ptr)=(uint8_t)(val),                       \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8))     \


/** 符号なし4Byteをポインタ参照先にホスト形式からリトルエンディアンでセットします。
 */
#define X_STORE_U32_LIT(ptr, val)                                   \
            (*(uint8_t*)(ptr)=(uint8_t)(val),                       \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8),     \
            *((uint8_t*)(ptr)+2)=(uint8_t)((uint32_t)(val)>>16),    \
            *((uint8_t*)(ptr)+3)=(uint8_t)((uint32_t)(val)>>24))


/** 符号なし2Byteをポインタ参照先にホスト形式からビッグエンディアンでセットします。
 */
#define X_STORE_U16_BIG(ptr, val)                                   \
            (*((uint8_t*)(ptr)+1)=(uint8_t)(val),                   \
            *((uint8_t*)(ptr))=(uint8_t)((uint16_t)(val)>>8))       \


/** 符号なし4Byteをポインタ参照先にホスト形式からビッグエンディアンでセットします。
 */
#define X_STORE_U32_BIG(ptr, val)                                   \
            (*((uint8_t*)(ptr)+3)=(uint8_t)(val),                   \
            *((uint8_t*)(ptr)+2)=(uint8_t)((uint16_t)(val)>>8),     \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint32_t)(val)>>16),    \
            *((uint8_t*)(ptr)+0)=(uint8_t)((uint32_t)(val)>>24))


/** void*引数end, beginのバイト単位のアドレス差を返します。
 */
static inline ptrdiff_t x_distance_ptr(const void* begin, const void* end)
{
    return (const char*)(end) - (const char*)(begin);
}


/** (begin <= x) && (x < end)を判定します。
 */
static inline bool x_is_within(int32_t x, int32_t begin, int32_t end)
{
    return ((begin <= x) && (x < end));
}


/** (begin <= x) && (x < end)を判定します。
 */
static inline bool x_is_uwithin(uint32_t x, uint32_t begin, uint32_t end)
{
    return ((begin <= x) && (x < end));
}


/** ptrが指すアドレスがbegin とendの範囲内かどうかをBool値で返します。
 */
static inline bool x_is_within_ptr(const void* ptr, const void* begin, const void* end)
{
    const char* const p = (const char*)(ptr);
    const char* const b = (const char*)(begin);
    const char* const e = (const char*)(end);

    return ((b <= p) && (p < e));
}


/// @cond IGNORE
#define X_DECLARE_LSB_POS_TABLE            \
    const uint8_t lsb_pos_table[15] =       \
        {                                   \
            0, /* (0001 & 1 << 0) != 0 */   \
            1, /* (0010 & 1 << 1) != 0 */   \
            0, /* (0011 & 1 << 0) != 0 */   \
            2, /* (0100 & 1 << 2) != 0 */   \
            0, /* (0101 & 1 << 0) != 0 */   \
            1, /* (0110 & 1 << 1) != 0 */   \
            0, /* (0111 & 1 << 0) != 0 */   \
            3, /* (1000 & 1 << 3) != 0 */   \
            0, /* (1001 & 1 << 0) != 0 */   \
            1, /* (1010 & 1 << 1) != 0 */   \
            0, /* (1011 & 1 << 0) != 0 */   \
            2, /* (1100 & 1 << 2) != 0 */   \
            0, /* (1101 & 1 << 0) != 0 */   \
            1, /* (1110 & 1 << 1) != 0 */   \
            0, /* (1111 & 1 << 0) != 0 */   \
        };
/// @endcond IGNORE


/** 下位から最も近くにセットされたビット位置を返します。
 *
 *  @attention
 *  1つ以上のビットがセットされていることを前提としています。
 */
static inline int x_find_lsb_pos8(uint8_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


static inline int x_find_lsb_pos16(uint16_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x00ff))     { x >>=  8; n +=  8;}
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


static inline int x_find_lsb_pos32(uint32_t x)
{
    X_DECLARE_LSB_POS_TABLE;
    int n = 0;
    if (!(x & 0x0000ffff)) { x >>= 16; n += 16;}
    if (!(x & 0x00ff))     { x >>=  8; n +=  8;}
    if (!(x & 0x0f))       { x >>=  4; n +=  4;}
    return n + lsb_pos_table[(x &0x0f) - 1];
}


/** 下位から最も近くにセットされたビットを返します。
 */
static inline uint8_t  x_find_lsb8(uint8_t x)  { return x & ((~x) + 1); }
static inline uint16_t x_find_lsb16(uint16_t x) { return x & ((~x) + 1); }
static inline uint32_t x_find_lsb32(uint32_t x) { return x & ((~x) + 1); }


/// @cond IGNORE
#define X_DECLARE_MSB_POS_TABLE            \
const uint8_t msb_pos_table[15] =           \
    {                                       \
        0,  /* (0001 & 1 << 0) != 0 */      \
        1,  /* (0010 & 1 << 1) != 0 */      \
        1,  /* (0011 & 1 << 1) != 0 */      \
        2,  /* (0100 & 1 << 2) != 0 */      \
        2,  /* (0101 & 1 << 2) != 0 */      \
        2,  /* (0110 & 1 << 2) != 0 */      \
        2,  /* (0111 & 1 << 2) != 0 */      \
        3,  /* (1000 & 1 << 3) != 0 */      \
        3,  /* (1001 & 1 << 3) != 0 */      \
        3,  /* (1010 & 1 << 3) != 0 */      \
        3,  /* (1011 & 1 << 3) != 0 */      \
        3,  /* (1100 & 1 << 3) != 0 */      \
        3,  /* (1101 & 1 << 3) != 0 */      \
        3,  /* (1110 & 1 << 3) != 0 */      \
        3,  /* (1111 & 1 << 3) != 0 */      \
    };
/// @endcond IGNORE


/** 上位から最も近くにセットされたビット位置を返します。
 */
static inline int x_find_msb_pos32(uint32_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xffff0000) { x >>= 16; n += 16;}
    if (x & 0xff00)     { x >>=  8; n +=  8;}
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


static inline int x_find_msb_pos16(uint16_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xff00)     { x >>=  8; n +=  8;}
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


static inline int x_find_msb_pos8(uint8_t x)
{
    X_DECLARE_MSB_POS_TABLE;
    int n = 0;
    if (x & 0xf0)       { x >>=  4; n +=  4;}
    return n + msb_pos_table[(x &0x0f) - 1];
}


/** 上位から最も近くにセットされたビットを返します。
 */
static inline uint8_t  x_find_msb8(uint8_t x)   { return 1U  << x_find_msb_pos8(x); }
static inline uint16_t x_find_msb16(uint16_t x) { return 1U  << x_find_msb_pos16(x); }
static inline uint32_t x_find_msb32(uint32_t x) { return 1UL << x_find_msb_pos32(x); }


/// @cond IGNORE
#define X_COUNT_BITS_IMPL(x) int count = 0; while(x) { ++count; x = x & (x - 1); } return count;
/// @endcond IGNORE


/** セットされたビット数を返します。
 */
static inline int x_count_bits8(uint8_t x)  { X_COUNT_BITS_IMPL(x); };
static inline int x_count_bits16(uint16_t x) { X_COUNT_BITS_IMPL(x); };
static inline int x_count_bits32(uint32_t x) { X_COUNT_BITS_IMPL(x); };


/** |A|B| -> |B|A|
 */
static inline void x_reverse_2byte(void* x)
{
    uint8_t* p = (uint8_t*)(x);
    uint8_t tmp;
    tmp    = *p;
    *p     = *(p+1);
    *(p+1) = tmp;
}


/** |A|B|C|D| -> |D|C|B|A|
 */
static inline void x_reverse_4byte(void* x)
{
    uint8_t* p = (uint8_t*)(x);
    uint8_t tmp;
    tmp    = *p;
    *p     = *(p+3);
    *(p+3) = tmp;
    tmp    = *(p+1);
    *(p+1) = *(p+2);
    *(p+2) = tmp;
}


/** @see http://www.musashinodenpa.com/arduino/ref/index.php?f=0&pos=2719
 */
static inline int32_t x_map(
        int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return ((int64_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/** x_map()の符号なし版です。
 */
static inline uint32_t x_umap(
        uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return ((uint64_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/** aとbからsizeバイトを入れ替えます。
 */
static inline void x_memswap(void *a, void *b, size_t size)
{
    char* ca;
    char* cb;
    char  tmp;
    for (ca = (char*)a, cb = (char*)b; size > 0; size--, ca++, cb++)
    {
        tmp = *ca;
        *ca  = *cb;
        *cb  = tmp;
    }
}


/** sizeバイトの要素n個からなる配列pの要素を逆順に並べ替えます。
 */
static inline void x_memreverse(void *p, size_t size, size_t n)
{
    char* l;
    char* r;

    l = (char*)p;
    r = (char*)p + ((n-1) * size);
    for ( ; l < r; l = l+size, r = r-size)
        x_memswap( l, r, size );
}


/** sizeバイトの要素n個からなる配列pの要素を右方向にshift個分回転移動させます。
 */
static inline void x_memrotate_right(void *p, size_t shift, size_t size, size_t n)
{
    x_memreverse(p,                      size, n);          // 012345678 -> 876543210
    x_memreverse(p,                      size, shift);      // 876543210 -> 678543210
    x_memreverse((char*)p+(size*shift),  size, n-shift);    // 678543210 -> 678012345
}


/** sizeバイトの要素n個からなる配列pの要素を左方向にshift個分回転移動させます。
 */
static inline void x_memrotate_left(void *p, size_t shift, size_t size, size_t n)
{
    x_memreverse(p,                      size, shift);      // 012345678 -> 210345678
    x_memreverse((char*)p+(size*shift),  size, n-shift);    // 210345678 -> 210876543
    x_memreverse(p,                      size, n);          // 210876543 -> 345678012
}


/** srcからdstにメモリの矩形コピーを行います。
 *
 *  @param dst      転送先
 *  @param src      転送元
 *  @param linesize 転送幅
 *  @param height   転送高さ
 *  @param dstride  転送先の次のラインまでの幅
 *  @param sstride  転送元の次のラインまでの幅
 *
 *  @attention
 *  srcとdstの領域は重なっていないことを前提とします。
 */
static inline void
x_memblt(void* dst, const void* src,
         size_t linesize, size_t height,
         size_t dstride,  size_t sstride)
{
    char* cdst = dst;
    const char* csrc = src;

    size_t y;
    for (y = 0; y < height; ++y)
    {
        memcpy(cdst, csrc, linesize);
        cdst += dstride;
        csrc += sstride;
    }
}


/** バイトオーダーがビッグエンディアンかどうかを返します。
 */
static inline bool x_is_big_endian(void)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return true;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return false;
#else
    union
    {
        uint32_t u;
        uint8_t  c[4];
    } e = {0x01000000};
    return e.c[0];
#endif
}


/** バイトオーダーがリトルエンディアンかどうかを返します。
 */
static inline bool x_is_little_endian(void)
{
    return ! x_is_big_endian();
}


/** 2Byteビッグエンディアンデータをホストのエンディアンにして返します。
 */
static inline uint16_t x_big_to_host16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian16(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian16(x);
#endif
}


/** 4Byteビッグエンディアンデータをホストのエンディアンにして返します。
 */
static inline uint32_t x_big_to_host32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian32(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian32(x);
#endif
}


/** 2Byteリトルエンディアンデータをホストのエンディアンにして返します。
 */
static inline uint16_t x_little_to_host16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian16(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian16(x);
#endif
}


/** 4Byteリトルエンディアンデータをホストのエンディアンにして返します。
 */
static inline uint32_t x_little_to_host32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian32(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian32(x);
#endif
}


/** 2Byteホストエンディアンデータをビッグエンディアンにして返します。
 */
static inline uint16_t x_host_to_big16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian16(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian16(x);
#endif
}


/** 4Byteホストエンディアンデータをビッグエンディアンにして返します。
 */
static inline uint32_t x_host_to_big32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x_reverse_endian32(x);
#else
    return x_is_big_endian() ? x : x_reverse_endian32(x);
#endif
}


/** 2Byteホストエンディアンデータをリトルエンディアンにして返します。
 */
static inline uint16_t x_host_to_little16(uint16_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian16(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian16(x);
#endif
}


/** 4Byteホストエンディアンデータをリトルエンディアンにして返します。
 */
static inline uint32_t x_host_to_little32(uint32_t x)
{
#if X_BYTE_ORDER == X_BYTE_ORDER_LITTLE
    return x;
#elif X_BYTE_ORDER == X_BYTE_ORDER_BIG
    return x_reverse_endian32(x);
#else
    return x_is_little_endian() ? x : x_reverse_endian32(x);
#endif
}


#ifdef __cplusplus
}
#endif


#endif // picox_core_utils_h_
