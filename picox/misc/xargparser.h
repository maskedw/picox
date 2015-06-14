/**
 *       @file  xargparser.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015�N06��14��
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


#ifndef xargparser_h_
#define xargparser_h_


#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifndef X_ARG_PARSER_MAX_TOKEN_SIZE
    #define X_ARG_PARSER_MAX_TOKEN_SIZE 256
#endif


typedef enum XArgParserErr
{
    X_ARG_PARSER_ERR_NONE,

    /* �N�I�[�g�������Ă��Ȃ� */
    X_ARG_PARSER_ERR_QUATE,

    /* �K��̍ő�l�𒴂��� */
    X_ARG_PARSER_ERR_OVERFLOW,

    /* �s���ȃG�X�P�[�v */
    X_ARG_PARSER_ERR_ESCAPE,
} XArgParserErr;


/** str����͂���argv��ݒ肵�܂��B
 *
 *  @param str      ��͕�����
 *  @param argc     �����̐��̏o�͐�
 *  @param argv     �����o�͐�Bmax_argc�ȏ�̗̈悪���邱�ƁB
 *  @param max_argc argc�̍ő�l
 *  @param endptr   ��͏������̏I���ʒu(NULL�w�莞�͉������Ȃ�)
 *
 *  @note
 *  str���g����͂ɂ���ăg�[�N�����Ƃ�'\0'�ŕ��f����邱�Ƃɒ��ӂ��Ă��������B
 *  ���[�h�I�����[�̕������n���Ă͂����܂���B
 */
XArgParserErr xargparser_to_argv(char* str, int* argc, char* argv[], size_t max_argc, char** endptr);


const char* xargparser_strerror(XArgParserErr err);


#ifdef __cplusplus
}
#endif


#endif /* xargparser_h_ */
