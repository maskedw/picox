/**
 *       @file  compiler.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/21
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


#ifndef picox_core_detail_xcompiler_h_
#define picox_core_detail_xcompiler_h_


/** @def    X_COMPILER_C99
 *  @brief  �R���p�C����C99�ɑΉ����Ă��邩�ǂ���
 */
#ifdef __STDC_VERSION__
    #if (__STDC_VERSION__ >= 199901L)
        #define X_COMPILER_C99 (1)
    #else
        #define X_COMPILER_C99 (0)
    #endif
#else
    #define X_COMPILER_C99 (0)
#endif


/** @def    X_GNUC_PREREQ
 *  @brief  GCC�̃o�[�W������n.m�ȏォ�ǂ�����Ԃ��܂�
 */
#if defined __GNUC__ && defined __GNUC_MINOR__
    #define X_GNUC_PREREQ(maj, min) \
        ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
    #define X_GNUC_PREREQ(maj, min) 0
#endif


#ifdef __GNUC__

#include <picox/core/detail/compiler/xgcc.h>

#endif


/** @def    X_INLINE
 *  @brief  �R���p�C���ɑ΂��āA�֐��̃C�����C���W�J���w������L�[���[�h�ł��B
 *
 *  ���ۂɃC�����C���W�J����邩�ǂ����́A�R���p�C�������f���܂��B
 */
#ifndef X_INLINE
    #define X_INLINE  static inline
#endif


/** @def    X_ALWAYS_INLINE
 *  @brief  �R���p�C���ɑ΂��āA�֐��̃C�����C���W�J����������L�[���[�h�ł��B
 *
 *  �œK�����x����A�֐��̓��e�Ɋւ�炸�A�K���C�����C���������悤�ɂ��܂��B
 */
#ifndef X_ALWAYS_INLINE
    #define X_ALWAYS_INLINE static inline
#endif


/** @def    X_HAS_VARIADIC_MACROS
 *  @brief  �R���p�C�����ϒ������}�N���ɑΉ����Ă��邩�ǂ���
 */
#ifndef X_HAS_VARIADIC_MACROS
    #if X_COMPILER_C99 > 0
        #define X_HAS_VARIADIC_MACROS   (1)
    #endif
#endif


/** @def    X_HAS_TYPEOF
 *  @brief  �R���p�C����typeof�g���ɑΉ����Ă��邩�ǂ���
 *
 *  X_HAS_TYPEOF == 1�̏ꍇ�AX_TYPEOF()���g�p�\�ł��B
 */
#ifndef X_HAS_TYPEOF
    #define X_HAS_TYPEOF    (0)
#endif


/** @def    X_LIKELY
 *  @brief  ��������Ɏg�p����R���p�C���œK���f�B���N�e�B�u�ł�
 *
 *  �Q�l�T�C�g<br>
 *  http://d.hatena.ne.jp/tkuro/20110114/1294956535
 */
#ifndef X_LIKELY
    #define X_LIKELY(x)         x
#endif


/** @def    X_UNLIKELY
 *  @brief  ��������Ɏg�p����R���p�C���œK���f�B���N�e�B�u�ł�
 *
 *  @see X_LIKELY
 */
#ifndef X_UNLIKELY
    #define X_UNLIKELY(x)         x
#endif


/** @def    X_HAS_STATEMENT_EXPRESSIONS
 *  @brief  {}�ň͂܂ꂽ�������ƕϐ��̐錾��()�̎��̒��Ŏg�p�ł��邩�ǂ���
 *
 *  ����ɑΉ����Ă���ƈȉ��̂悤�Ȗ����Ȃ��Ƃ��ł��܂��B
 *  @code
 *  if ({ func1();
 *        if (x) // if �̒��� if��������
 *          ...
 *        else
 *          ...
 *        1; // �Ō�̎���if�̔���Ɏg�p�����
 *        })
 *      ...
 *  @endcode
 */
#ifndef X_HAS_STATEMENT_EXPRESSIONS
    #define X_HAS_STATEMENT_EXPRESSIONS (0)
#endif


/** @def    X_MEMORY_BARRIER
 *  @brief  ����������̏������𐧌䂷��CPU���߂ł�
 */
#ifndef X_MEMORY_BARRIER
    #define X_MEMORY_BARRIER
#endif


/** @def    X_UNREACHABE
 *  @brief  �ȉ��̃R�[�h�ɂ͌����ē��B���Ȃ����Ƃ�\������R���p�C���f�B���N�e�B�u�ł�
 */
#ifndef X_UNREACHABE
    #define X_UNREACHABE    for (;;)
#endif


/** @def    X_DEPRECATED
 *  @brief  �w��̒�`���p�~�\���񐄏��ł��邱�Ƃ�\������R���p�C���f�B���N�e�B�u�ł�
 *
 *  �R���p�C�������̋@�\�ɑΉ����Ă���΁ADEPRECATED�w��̒�`���g�p����Ă����
 *  ���A�R���p�C�����Ɍx�����o�͂���܂��B
 *  �֐��ȊO�ɂ��A�^��`��A�ϐ��ɂ��g�p���邱�Ƃ��ł��܂��B
 *
 *  @code
 *  X_DEPRECATED void deprecated_func(void);
 *  X_DEPRECATED typedef int deprecated_type_t;
 *  @endcode
 */
#ifndef X_DEPRECATED
    #define X_DEPRECATED
#endif


#ifndef NULL
    #define NULL    ((void*)0)
#endif


#ifndef EOF
    #define EOF     (-1)
#endif


#endif // picox_core_detail_xcompiler_h_
