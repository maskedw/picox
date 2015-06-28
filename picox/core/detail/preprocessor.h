/**
 *       @file  preprocessor.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/27
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


#ifndef picox_core_detail_preprocessor_h_
#define picox_core_detail_preprocessor_h_


/// @cond IGNORE
#define X_PP_CAT__(a, b)          a ## b
/// @endcond IGNORE

/** �g�[�N��a, b��A�����܂��B
 */
#define X_PP_CAT(a, b)            X_PP_CAT__(a, b)


/** �g�[�N��a, b, c��A�����܂��B
 */
#define X_PP_CAT3(a,b,c)          X_PP_CAT(X_PP_CAT(a,b),c)


/** �g�[�N��a, b, c, d��A�����܂��B
 */
#define X_PP_CAT4(a,b,c,d)        X_PP_CAT(X_PP_CAT3(a,b,c),d)


/** �g�[�N��a, b, c, d, e��A�����܂��B
 */
#define X_PP_CAT5(a,b,c,d,e)      X_PP_CAT(X_PP_CAT4(a,b,c,d),e)


/// @cond IGNORE
#define X_PP_STRINGIZE__(text)    #text
/// @endcond IGNORE

/** �g�[�N��text�𕶎��񉻂��܂��B
 *
 *  @note
 *  X_PP_STRINGIZE__()��ʂ���2�i�K�̃}�N�����g�p���Ă���̂́Atext��define���ꂽ
 *  �g�[�N����n���ꂽ���ɁAdefine��W�J������̒l�ŕ����񉻂��邽�߂ł��B
 */
#define X_PP_STRINGIZE(text)      X_PP_STRINGIZE__(text)


#endif // picox_core_detail_preprocessor_h_
