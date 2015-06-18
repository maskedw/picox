/**
 *       @file  xutils.h
 *      @brief  �჌�x���Ȃ��ꂱ��G���ȃ��[�e�B���e�B�W
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

#ifndef picox_xutils_h_
#define picox_xutils_h_


/* std */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>


#ifdef __cplusplus
extern "C" {
#endif


#define X_BYTE_ORDER_LITTLE    (0)
#define X_BYTE_ORDER_BIG       (1)
#define X_BYTE_ORDER_UNKNOWN   (2)


#ifndef X_BYTE_ORDER
    #define X_BYTE_ORDER       X_BYTE_ORDER_UNKNOWN
#endif


/** �g���݌^�̍ő�A���C�����g�^�ł��B
 */
typedef union XMaxAlign
{
    char c; int i; long l; long long ll; void* p;
    float f; double d; long double ld; void(*fp)(void);
} XMaxAlign;


/** �ő�T�C�Y�̃A���C�����g��size�o�C�g�ȏ�̗̈�����ϐ�name���`���܂��B
 */
#define X_DEF_MAX_ALIGNED(name, size) XMaxAlign name[((size) + sizeof(XMaxAlign) - 1) / sizeof(XMaxAlign)]


/** �\���̂⋤�p�̃����o��sizeof��Ԃ��܂��B
 */
#define X_SIZEOF_MEM(s, m) (sizeof(((s*)0)->m))


/** �\���̂⋤�p�̃����o�̐擪����̃I�t�Z�b�g��Ԃ��܂��B
 */
#define X_OFFSET_OF(s, m)   ((uintptr_t)&(((s *)0)->m))


/** �^type�̃A���C�����g��Ԃ��܂��B
 */
#define X_ALIGN_OF(type)   X_OFFSET_OF(struct { char c; type member; }, member)


/** �R���p�C�����A�T�[�g���s���܂��B
 */
#define X_STATIC_ASSERT(cond)  \
    enum { X_CAT(X_STATIC_ASSERTION_FAILED, __LINE__) = \
          sizeof( struct { int assertion_failed[(cond) ? 1: -1];})}


/** �g�[�N��a, b��A�����܂��B
 */
#define X_CAT(a, b)            X_CAT__(a, b)


/** �g�[�N��a, b, c��A�����܂��B
 */
#define X_CAT3(a,b,c)          X_CAT(X_CAT(a,b),c)


/** �g�[�N��a, b, c, d��A�����܂��B
 */
#define X_CAT4(a,b,c,d)        X_CAT(X_CAT3(a,b,c),d)


/** �g�[�N��a, b, c, d, e��A�����܂��B
 */
#define X_CAT5(a,b,c,d,e)      X_CAT(X_CAT4(a,b,c,d),e)

/// @cond IGNORE
#define X_CAT__(a, b)          a ## b
/// @endcond IGNORE


/** �g�[�N��text�𕶎��񉻂��܂��B
 *
 *  @note
 *  X_STRINGIZE__()��ʂ���2�i�K�̃}�N�����g�p���Ă���̂́Atext��define���ꂽ
 *  �g�[�N����n���ꂽ���ɁAdefine��W�J������̒l�ŕ����񉻂��邽�߂ł��B
 */
#define X_STRINGIZE(text)      X_STRINGIZE__(text)

/// @cond IGNORE
#define X_STRINGIZE__(text)    #text
/// @endcond IGNORE


/** bit x��Ԃ��܂��B
 */
#define X_BIT(x)       (1UL << (x))


/** a,b���r���A����������Ԃ��܂��B
 */
#define X_MIN(a,b)     (((a)<(b))?(a):(b))


/** a,b���r���A�傫������Ԃ��܂��B
 */
#define X_MAX(a,b)     (((a)>(b))?(a):(b))


/** cond == true�̎��Abreak�������s���܂��B
 */
#define X_BREAK_IF(cond)   if(cond) break


/** cond == true�̎��Alabel�փW�����v����goto�������s���܂��B
 */
#define X_GOTO_IF(cond, label)   if(cond) goto label


/** �R���p�C���ɂ�関�g�p�ϐ��̌x����}������}�N���ł��B
 */
#define X_UNUSED(x)    (void)(x)


/** �������[�v��\������}�N���ł��B
 */
#define X_FOREVER()    for (;;)


/** x�r�b�g�ڂ̃r�b�g��Ԃ��܂��B
 */
#define X_BIT(x) (1UL << (x))


/** �ꎟ���z��̗v�f����Ԃ��܂��B
 */
#define X_COUNT_OF(a)      (sizeof(a) / sizeof(*(a)))


/** �񎟌��z��̍s�v�f����Ԃ��܂��B
 */
#define X_COUNT_OF_ROW(x) (sizeof(x) / sizeof(x[0]))


/** �񎟌��z��̗�v�f����Ԃ��܂��B
 */
#define X_COUNT_OF_COL(x) (sizeof(x[0]) / sizeof(x[0][0]))


/** �񎟌��z��̗v�f����Ԃ��܂��B
 */
#define X_COUNT_OF_2D(x)   (X_COUNT_OF_ROW(x) * X_COUNT_OF_COL(x))


/** x��m�̔{���ɐ؂�グ���l��Ԃ��܂��B
 */
#define X_ROUNDUP_MULTIPLE(x, m) ((((uint32_t)x) + (m) - 1) & ((uint32_t)0 - (m)))


/** x��2�̔{���ɐ؂�グ���l��Ԃ��܂��B
 */
#define X_ROUNDUP_MULTIPLE_OF_TWO(x) (X_ROUNDUP_MULTIPLE((x), 2))


/// @cond IGNORE
#define X_ROUNDUP_POWER_OF_TWO_5(x) (((x) | (x) >> 16) + 1)
#define X_ROUNDUP_POWER_OF_TWO_4(x) X_ROUNDUP_POWER_OF_TWO_5((x) | ((x) >> 8))
#define X_ROUNDUP_POWER_OF_TWO_3(x) X_ROUNDUP_POWER_OF_TWO_4((x) | ((x) >> 4))
#define X_ROUNDUP_POWER_OF_TWO_2(x) X_ROUNDUP_POWER_OF_TWO_3((x) | ((x) >> 2))
#define X_ROUNDUP_POWER_OF_TWO_1(x) X_ROUNDUP_POWER_OF_TWO_2((x) | ((x) >> 1))
/// @endcond IGNORE


/** x���ł��߂�2�ׂ̂���ɐ؂�グ���l��Ԃ��܂��B
 */
#define X_ROUNDUP_POWER_OF_TWO(x)   X_ROUNDUP_POWER_OF_TWO_1((x) - 1)


/** x��m�̔{���ɐ؂艺�����l��Ԃ��܂��B
 */
#define X_ROUNDDOWN_MULTIPLE(x, m)   ((x) - ((x) % (m)))


/** x��2�̔{���ɐ؂艺�����l��Ԃ��܂��B
 */
#define X_ROUNDDOWN_MULTIPLE_OF_TWO(x)  (X_ROUNDDOWN_MULTIPLE((x), 2))


/// @cond IGNORE
#define X_ROUNDDOWN_POWER_OF_TWO_5(x) ((x) - ((x) >> 1))
#define X_ROUNDDOWN_POWER_OF_TWO_4(x) X_ROUNDDOWN_POWER_OF_TWO_5((x) | ((x) >> 16))
#define X_ROUNDDOWN_POWER_OF_TWO_3(x) X_ROUNDDOWN_POWER_OF_TWO_4((x) | ((x) >> 8))
#define X_ROUNDDOWN_POWER_OF_TWO_2(x) X_ROUNDDOWN_POWER_OF_TWO_3((x) | ((x) >> 4))
#define X_ROUNDDOWN_POWER_OF_TWO_1(x) X_ROUNDDOWN_POWER_OF_TWO_2((x) | ((x) >> 2))
/// @endcond IGNORE


/** x���ł��߂�2�ׂ̂���ɐ؂艺�����l��Ԃ��܂��B
 */
#define X_ROUNDDOWN_POWER_OF_TWO(x)   X_ROUNDDOWN_POWER_OF_TWO_1((x) | ((x) >> 1))


/** x��m�̔{�����ǂ�����Bool�l�ŕԂ��܂��B
 */
#define X_IS_MULTIPLE(x, m)  (X_ROUNDUP_MULTIPLE(x, m) == (x))


/** x��2�̔{�����ǂ�����Bool�l�ŕԂ��܂��B
 */
#define X_IS_MULTIPLE_OF_TWO(x)  (X_ROUNDUP_MULTIPLE_OF_TWO(x) == (x))


/** x��2�ׂ̂��悩�ǂ�����Bool�l�ŕԂ��܂��B
 */
#define X_IS_POWER_OF_TWO(x)   (((x) & -(x)) == (x))


/** x�̏��16bit�̒l��Ԃ��܂��B
 */
#define X_HIGH_WORD(x) ((uint16_t)((x) >> 16))


/** x�̉���16bit�̒l��Ԃ��܂��B
 */
#define X_LOW_WORD(x) ((uint16_t)(x))


/** x�̏��8bit�̒l��Ԃ��܂��B
 */
#define X_HIGH_BYTE(x) (((uint8_t)(x)) >> 8)


/** x�̉���8bit�̒l��Ԃ��܂��B
 */
#define X_LOW_BYTE(x) ((uint8_t)(x))


/** x�̏��4bit�̒l��Ԃ��܂��B
 */
#define X_HIGH_NIBBLE(x) (((uint8_t)(x)) >> 4)


/** x�̉���4bit�̒l��Ԃ��܂��B
 */
#define X_LOW_NIBBLE(x) (((uint8_t)(x)) & 0x0f)


/// @cond IGNORE
#define X_REVERSE_BITS8__(x)                      (((((x) & 0x55) << 1) | (((x) & 0xaa) >> 1)))
#define X_REVERSE_BITS8_(x)   (X_REVERSE_BITS8__  ((((x) & 0x33) << 2) | (((x) & 0xcc) >> 2)))
/// @endcond IGNORE


/** x�̉���8bit���t�]�����l��Ԃ��܂��B
 */
#define X_REVERSE_BITS8(x)    (X_REVERSE_BITS8_  (((uint8_t)(x)) << 4) |  (((uint8_t)(x)) >> 4))


/** x�̉���16bit���t�]�����l��Ԃ��܂��B
 */
#define X_REVERSE_BITS16(x)   (((uint16_t)(X_REVERSE_BITS8(ME_HIGH_BYTE(x)))) | \
                               (((uint16_t) X_REVERSE_BITS8(X_LOW_BYTE(x)))  << 8))


/** x�̉���32bit���t�]�����l��Ԃ��܂��B
 */
#define X_REVERSE_BITS32(x)   (((uint32_t)(X_REVERSE_BITS16(X_HIGH_WORD(x)))) | \
                               (((uint32_t)X_REVERSE_BITS16(X_LOW_WORD(x))) << 16))


/** x�̉���2�o�C�g�̃o�C�g�I�[�_�[���t�]�����l��Ԃ��܂��B
 */
#define X_REVERSE_ENDIAN16(x)  ((((uint16_t)(x)) << 8) | (((uint16_t)(x) >> 8) & 0x00ff))


/** x�̉���4�o�C�g�̃o�C�g�I�[�_�[���t�]�����l��Ԃ��܂��B
 */
#define X_REVERSE_ENDIAN32(x) (((((uint32_t)(x)) << 24)     | \
                                (((x) <<  8) & 0x00ff0000)   | \
                                (((x) >>  8) & 0x0000ff00)   | \
                                (((x) >> 24) & 0x000000ff)))


/** x�̉���8bit����r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_ODD_BITS8(x)  (((uint8_t)(x)) & 0x55)


/** x�̉���16bit����r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_ODD_BITS16(x) (((uint16_t)(x)) & 0x5555)


/** x�̉���32bit����r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_ODD_BITS32(x) (((uint32_t)(x)) & 0x55555555)


/** x�̉���8bit�������r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_EVEN_BITS8(x)  (((uint8_t)(x)) & 0xaa)


/** x�̉���16bit�������r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_EVEN_BITS16(x) (((uint16_t)(x)) & 0xaaaa)


/** x�̉���32bit�������r�b�g�Ń}�X�N�����l��Ԃ��܂��B
 */
#define X_EVEN_BITS32(x) (((uint32_t)(x)) & 0xaaaaaaaa)


/** x�̉���8bit�ׂ̗荇�����r�b�g�����������l��Ԃ��܂��B
 */
#define X_SWAP_ADJACENT_BITS8(x)  ((X_ODD_BITS8(x) << 1) | ((X_EVEN_BITS8(x)) >> 1))


/** x�̉���16bit�ׂ̗荇�����r�b�g�����������l��Ԃ��܂��B
 */
#define X_SWAP_ADJACENT_BITS16(x) ((X_ODD_BITS16(x) << 1) | ((X_EVEN_BITS16(x)) >> 1))


/** x�̉���32bit�ׂ̗荇�����r�b�g�����������l��Ԃ��܂��B
 */
#define X_SWAP_ADJACENT_BITS32(x) ((X_ODD_BITS32(x) << 1) | ((X_EVEN_BITS32(x)) >> 1))


/** �o�C�i���Z�[�t��8bit�f�[�^��Ԃ��܂��B
 */
#define X_LD_U8(ptr)  (uint8_t)(*(uint8_t*)(ptr))


/** �o�C�i���Z�[�t��16bit���g���G���f�B�A���f�[�^��Ԃ��܂��B
 */
#define X_LD_U16_LIT(ptr) (uint16_t)(((uint16_t)*((uint8_t*)(ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr))


/** �o�C�i���Z�[�t��32bit���g���G���f�B�A���f�[�^��Ԃ��܂��B
 */
#define X_LD_U32_LIT(ptr)                                          \
        (uint32_t)(((uint32_t)*((uint8_t*)(ptr)+3)<<24)|            \
                  ((uint32_t)*((uint8_t*)(ptr)+2)<<16) |            \
                  ((uint16_t)*((uint8_t*)(ptr)+1)<<8)  |            \
                  *(uint8_t*)(ptr))


/** �o�C�i���Z�[�t��16bit�r�b�O�G���f�B�A���f�[�^��Ԃ��܂��B
 */
#define X_LD_U16_BIG(ptr) (uint16_t)(((uint16_t)(*((uint8_t*)(ptr)))<<8)|(uint16_t)*((uint8_t*)(ptr) + 1))


/** �o�C�i���Z�[�t��32bit�r�b�O�G���f�B�A���f�[�^��Ԃ��܂��B
 */
#define X_LD_U32_BIG(ptr)                                         \
        (uint32_t)(((uint32_t)*((uint8_t*)(ptr)+0)<<24)|           \
                  ((uint32_t)*((uint8_t*)(ptr)+1)<<16) |           \
                  ((uint16_t)*((uint8_t*)(ptr)+2)<<8)  |           \
                  *(uint8_t*)((ptr) + 3))


/** �o�C�i���Z�[�t��8bit�f�[�^���Z�b�g���܂��B
 */
#define X_ST_U8(ptr, val)    *(uint8_t*)(ptr)=(uint8_t)(val)


/** �o�C�i���Z�[�t��16bit�f�[�^�����g���G���f�B�A���ŃZ�b�g���܂��B
 */
#define X_ST_U16_LIT(ptr, val)                                             \
        do {                                                                \
            *(uint8_t*)(ptr)=(uint8_t)(val);                                \
            *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8);             \
        } while(0)


/** �o�C�i���Z�[�t��32bit�f�[�^�����g���G���f�B�A���ŃZ�b�g���܂��B
 */
#define X_ST_U32_LIT(ptr, val)                                             \
    do {                                                                    \
        *(uint8_t*)(ptr)=(uint8_t)(val);                                    \
        *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8);                 \
        *((uint8_t*)(ptr)+2)=(uint8_t)((uint32_t)(val)>>16);                \
        *((uint8_t*)(ptr)+3)=(uint8_t)((uint32_t)(val)>>24);                \
    } while(0)


/** �o�C�i���Z�[�t��16bit�f�[�^���r�b�O�G���f�B�A���ŃZ�b�g���܂��B
 */
#define X_ST_U16_BIG(ptr, val)                                             \
        do {                                                                \
            *((uint8_t*)(ptr)+1)=(uint8_t)(val);                            \
            *((uint8_t*)(ptr))=(uint8_t)((uint16_t)(val)>>8);               \
        } while(0)


/** �o�C�i���Z�[�t��32bit�f�[�^���r�b�O�G���f�B�A���ŃZ�b�g���܂��B
 */
#define X_ST_U32_BIG(ptr, val)                                             \
    do {                                                                    \
        *((uint8_t*)(ptr)+3)=(uint8_t)(val);                                \
        *((uint8_t*)(ptr)+2)=(uint8_t)((uint16_t)(val)>>8);                 \
        *((uint8_t*)(ptr)+1)=(uint8_t)((uint32_t)(val)>>16);                \
        *((uint8_t*)(ptr)+0)=(uint8_t)((uint32_t)(val)>>24);                \
    } while(0)


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


/** void*����end, begin�̃o�C�g�P�ʂ̃A�h���X����Ԃ��܂��B
 */
static inline ptrdiff_t x_distance_addr(const void* begin, const void* end)
{
    return (const char*)(end) - (const char*)(begin);
}


/** ptr���w���A�h���X��1�܂���2�ׂ̂��悩�ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_alignment(const void* ptr)
{
    const uintptr_t uptr = (uintptr_t)ptr;
    return (uptr > 0) && ((uptr & (uptr - 1)) == 0);
}


/** ptr���w���A�h���X��alignment�̔{�����ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_aligned(const void* ptr, size_t alignment)
{
    const uintptr_t uptr = (uintptr_t)ptr;
    return X_IS_MULTIPLE(uptr, alignment);
}


/** (begin <= x) && (x < end)�𔻒肵�܂��B
 */
static inline bool x_is_within(int32_t x, int32_t begin, int32_t end)
{
    return ((begin <= x) && (x < end));
}


/** (begin <= x) && (x < end)�𔻒肵�܂��B
 */
static inline bool x_is_uwithin(uint32_t x, uint32_t begin, uint32_t end)
{
    return ((begin <= x) && (x < end));
}


/** ptr���w���A�h���X��begin ��end�͈͓̔����ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_within_addr(const void* ptr, const void* begin, const void* end)
{
    const char* const p = (const char*)(ptr);
    const char* const b = (const char*)(begin);
    const char* const e = (const char*)(end);

    return ((b <= p) && (p < e));
}


/** ���ʂ���ł��߂��ɃZ�b�g���ꂽ�r�b�g�ʒu��Ԃ��܂��B
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


/** ���ʂ���ł��߂��ɃZ�b�g���ꂽ�r�b�g��Ԃ��܂��B
 */
static inline uint8_t  x_find_lsb8(uint8_t x)  { return x & ((~x) + 1); }
static inline uint16_t x_find_lsb16(uint16_t x) { return x & ((~x) + 1); }
static inline uint32_t x_find_lsb32(uint32_t x) { return x & ((~x) + 1); }


/** ��ʂ���ł��߂��ɃZ�b�g���ꂽ�r�b�g�ʒu��Ԃ��܂��B
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


/** ��ʂ���ł��߂��ɃZ�b�g���ꂽ�r�b�g��Ԃ��܂��B
 */
static inline uint8_t  x_find_msb8(uint8_t x)   { return 1U  << x_find_msb_pos8(x); }
static inline uint16_t x_find_msb16(uint16_t x) { return 1U  << x_find_msb_pos16(x); }
static inline uint32_t x_find_msb32(uint32_t x) { return 1UL << x_find_msb_pos32(x); }


/// @cond IGNORE
#define X_COUNT_BITS_IMPL(x) int count = 0; while(x) { ++count; x = x & (x - 1); } return count;
/// @endcond IGNORE


/** �Z�b�g���ꂽ�r�b�g����Ԃ��܂��B
 */
static inline int x_count_bits8(uint8_t x)  { X_COUNT_BITS_IMPL(x); };
static inline int x_count_bits16(uint16_t x) { X_COUNT_BITS_IMPL(x); };
static inline int x_count_bits32(uint32_t x) { X_COUNT_BITS_IMPL(x); };


/** ����x�̃o�C�g�I�[�_�[���t�]�����l��Ԃ��܂��B
 */
static inline uint16_t x_reverse_endian16(uint16_t x) { return X_REVERSE_ENDIAN16(x); }
static inline uint32_t x_reverse_endian32(uint32_t x) { return X_REVERSE_ENDIAN32(x); }


/** ����x�̃r�b�g���t�]�����l��Ԃ��܂��B
 */
static inline uint8_t x_reverse_bits8(uint8_t x)
{
    X_DECLARE_BIT_REVERSE_TABLE;
    const uint8_t a = bit_reverse_table[(x >> 4)];
    const uint8_t b = bit_reverse_table[(x & 0xf)] << 4;
    return a | b;
}
static inline uint16_t x_reverse_bits16(uint16_t x)
{
    const uint16_t a = x_reverse_bits8(x >> 8);
    const uint16_t b = x_reverse_bits8(x) << 8;
    return a | b;
}
static inline uint32_t x_reverse_bits32(uint32_t x)
{
    const uint32_t a = x_reverse_bits16(x >> 16);
    const uint32_t b = ((uint32_t)x_reverse_bits16(x)) << 16;
    return a | b;
}


/** |A|B| -> |B|A|
 */
static inline void x_swap_2byte(void* x)
{
    uint8_t* p = (uint8_t*)(x);
    uint8_t tmp;
    tmp    = *p;
    *p     = *(p+1);
    *(p+1) = tmp;
}


/** |A|B|C|D| -> |D|C|B|A|
 */
static inline void x_swap_4byte(void* x)
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


/** x��m�̔{���ɐ؂�グ���l��Ԃ��܂��B
 */
static inline uint32_t x_roundup_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDUP_MULTIPLE(x, m);
}


static inline void* x_roundup_multiple_ptr(const void* x, uint32_t m)
{
    return (void*)(((uintptr_t)x + m - 1) & ((uintptr_t)0 - m));
}

static inline uintptr_t x_roundup_multiple_uintptr(uintptr_t x, uint32_t m)
{
    return (uintptr_t)(x_roundup_multiple_ptr((void*)x, m));
}

/** x��2�̔{���ɐ؂�グ���l��Ԃ��܂��B
 */
static inline uint32_t x_roundup_multiple_of_two(uint32_t x)
{
    return X_ROUNDUP_MULTIPLE_OF_TWO(x);
}


/** x���ł��߂�2�ׂ̂���ɐ؂�グ�܂��B
 */
static inline uint32_t x_roundup_power_of_two(uint32_t x)
{
    return X_ROUNDUP_POWER_OF_TWO(x);
}


/** x��m�̔{���ɐ؂艺�����l��Ԃ��܂�
 */
static inline uint32_t x_rounddown_multiple(uint32_t x, uint32_t m)
{
    return X_ROUNDDOWN_MULTIPLE(x, m);
}


/** x��2�̔{���ɐ؂艺�����l��Ԃ��܂��B
 */
static inline uint32_t x_rounddown_multiple_of_two(uint32_t x)
{
    return X_ROUNDDOWN_MULTIPLE_OF_TWO(x);
}


/** x��2�ׂ̂���ɐ؂艺�����l��Ԃ��܂�
 */
static inline uint32_t x_rounddown_power_of_two(uint32_t x)
{
    return X_ROUNDDOWN_POWER_OF_TWO(x);
}


/** x��m�̔{�����ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_multiple(uint32_t x, uint32_t m)
{
    return X_IS_MULTIPLE(x, m);
}


/** x��2�̔{�����ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_multiple_of_two(uint32_t x)
{
    return X_IS_MULTIPLE_OF_TWO(x) == x;
}


/** x��2�ׂ̂��悩�ǂ�����Bool�l�ŕԂ��܂��B
 */
static inline bool x_is_power_of_two(uint32_t x)
{
    return X_IS_POWER_OF_TWO(x);
}


/** @see http://www.musashinodenpa.com/arduino/ref/index.php?f=0&pos=2696
 */
static inline int32_t x_constrain(int32_t x, int32_t a, int32_t b)
{
    if (x < a) return a;
    else if (b < x) return b;
    return x;
}


/** @see http://www.musashinodenpa.com/arduino/ref/index.php?f=0&pos=2719
 */
static inline int32_t x_map(
        int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return ((int64_t)(x - in_min)) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/** ��`�R�s�[���s���܂��B
 */
static inline void x_copy_rect(
    void* dest, const void* src, int linesize, int height, int stride)
{
    char* cdest = (char*)(dest);
    const char* csrc = (const char*)(src);

    int y;
    for (y = 0; y < height; ++y)
    {
        memcpy(cdest, csrc, linesize);
        cdest += linesize;
        csrc += stride;
    }
}


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


static inline bool x_is_little_endian(void)
{
    return ! x_is_big_endian();
}


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


#endif // picox_xutils_h_
