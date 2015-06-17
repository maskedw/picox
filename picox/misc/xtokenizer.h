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

#ifndef xtokenizer_h_
#define xtokenizer_h_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifndef XTOK_MAX_NUM_COLS
    /** �ő�� */
    #define XTOK_MAX_NUM_COLS    10
#endif


#ifndef XTOK_MAX_COL_SIZE
    /** 1��̍ő啶���� */
    #define XTOK_MAX_COL_SIZE    128
#endif


#ifndef XTOK_ASSERT
    #define XTOK_ASSERT(expr)   do { if (! expr) for(;;); } while (0)
#endif


typedef struct XTokenizer
{
    char*       tokens[XTOK_MAX_NUM_COLS];
    int         ntokens;
} XTokenizer;


/** �w�肳�ꂽseparater�ōs�𕪊����܂��B
 *
 *  row��std::strtok()��낵���j�󂳂�邱�Ƃɒ��ӂ��Ă��������B
 *  �܂��AXTokenizer�I�u�W�F�N�g�̃��\�b�h���ĂԊԁArow�͕ێ������K�v�������
 *  ���B
 */
int xtok_parse_row(XTokenizer* tok, char* row, char separater);


/** �񐔂�Ԃ��܂��B
 */
int xtok_num_tokens(XTokenizer* tok);


/** �w���̗v�f��Ԃ��܂��B
 *
 *  �͈͊O���w�肳�ꂽ�ꍇ��NULL��Ԃ��܂��B
 */
const char* xtok_get_token(XTokenizer* tok, int col);


/** �w����int�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_int(XTokenizer* tok, int col, int def, int* dst);


/** �w����unsigned int�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_uint(XTokenizer* tok, int col, unsigned def, unsigned* dst);


/** �w����int32_t�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_int32(XTokenizer* tok, int col, int32_t def, int32_t* dst);


/** �w����uint32_t�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_uint32(XTokenizer* tok, int col, uint32_t def, uint32_t* dst);


/** �w����double�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_double(XTokenizer* tok, int col, double def, double* dst);


/** �w����float�ɕϊ����ĕԂ��܂��B
 */
bool xtok_to_float(XTokenizer* tok, int col, float def, float* dst);


/** �w���̕������Ԃ��܂��B
 *
 *  strip == true���͑O���std::isspace()�Ńg�������܂��B
 */
bool xtok_to_string(XTokenizer* tok, int col, const char* def, char* dst, size_t size, bool strip);


/** �w����bool�ɕϊ����ĕԂ��܂��B
 *
 *  + y, yes, true, 1 => true
 *  + n, no, false, 0 => false
 */
bool xtok_to_bool(XTokenizer* tok, int col, bool def, bool* dst);


#ifdef __cplusplus
}
#endif


#endif // xtokenizer_h_
