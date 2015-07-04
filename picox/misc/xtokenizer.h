/**
 *       @file  xtokenizer.h
 *      @brief
 *
 *    @details
 *    ��������w��̕����ŕ������A�w��̌^�ɕϊ����邽�߂̃C���^�[�t�F�[�X�����
 *    �����W���[���ł��B
 *    strtok()����舵���₷�����������ł��B
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

#ifndef picox_misc_xtokenizer_h_
#define picox_misc_xtokenizer_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct XTokenizer
{
/// @privatesection
    char*       row;
    char**      tokens;
    int         ntokens;
} XTokenizer;


/** ��������w�蕶���ŗ�ɕ������܂��B
 *
 *  ������̓I�u�W�F�N�g�ɃR�s�[����܂��B���������xtok_release()�ŕK�����\�[�X
 *  ����������Ă��������B
 *
 *  @param row          �s������
 *  @param separater    ��؂蕶��
 *  @param max_tokens   �ő��
 *  @retval false
 *      + �������m�ێ��s
 *      + �ő�񐔂𒴂���
 *
 *  @pre
 *  + row != NULL
 *  + max_tokens > 0
 */
bool xtok_init(XTokenizer* self, const char* row, char separater, int max_tokens);


/** �I�u�W�F�N�g���ێ����郊�\�[�X��������܂��B
 *
 *  @note
 *  xtok_init()�Ŏ��s��̌Ăяo���ł����S�ɓ��삵�܂��B
 */
void xtok_release(XTokenizer* self);


/** ����Q�Ƃ��܂�
 *
 *  @pre
 *  col <= (xtok_num_tokens() - 1)
 */
static inline const char*
xtok_ref_token(const XTokenizer* self, int col)
{
    X_ASSERT(self);
    X_ASSERT(x_is_within(col, 0, self->ntokens));
    return self->tokens[col];
}


/** �񐔂�Ԃ��܂�
 */
static inline int
xtok_num_tokens(const XTokenizer* self)
{
    X_ASSERT(self);
    return self->ntokens;
}

#ifdef __cplusplus
}
#endif


#endif // picox_misc_xtokenizer_h_
