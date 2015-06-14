/**
 *       @file  xtokenizer.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/03/06
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2014> <MaskedW [maskedw00@gmail.com]>
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

#include "xtokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define EQUAL_STR(s1, s2)           (strcmp(s1, s2) == 0)
#define BREAK_IF(cond)              if (cond) break


static char* X__Strip(char* str);
static uint32_t X__GetInt(XTokenizer* tok, bool* ok, int col, uint32_t def);
static double X__GetDouble(XTokenizer* tok, bool* ok, int col, double def);


int xtok_parse_row(XTokenizer* tok, char* row, char separater)
{
    if ((row == NULL) || (row[0] == '\0'))
    {
        tok->ntokens = 0;
        return 0;
    }

    tok->tokens[0] = row;
    tok->ntokens = 1;

    char* p1 = row;
    char* p2;
    while ((p2 = strchr(p1, separater)) != NULL)
    {
        if (tok->ntokens >= XTOK_MAX_NUM_COLS)
        {
            tok->ntokens = 0;
            return -1;
        }

        *p2 = '\0';
        p1 = p2 + 1;
        tok->tokens[tok->ntokens] = p1;
        tok->ntokens++;

    }
    return tok->ntokens;
}


int xtok_num_tokens(XTokenizer* tok)
{
    XTOK_ASSERT(tok);
    return tok->ntokens;
}


const char* xtok_get_token(XTokenizer* tok, int col)
{
    bool ok = false;

    do
    {
        BREAK_IF((col < 0) || (tok->ntokens <= col));
        BREAK_IF(tok->tokens[col] == '\0');
        ok = true;
    } while (0);

    return ok ? tok->tokens[col] : NULL;
}


bool xtok_get_int(XTokenizer* tok, int col, int def, int* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetInt(tok, &ok, col, def);

    return ok;
}


bool xtok_get_uint(XTokenizer* tok, int col, unsigned def, unsigned* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetInt(tok, &ok, col, def);

    return ok;
}


bool xtok_get_int32(XTokenizer* tok, int col, int32_t def, int32_t* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetInt(tok, &ok, col, def);

    return ok;
}


bool xtok_get_uint32(XTokenizer* tok, int col, uint32_t def, uint32_t* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetInt(tok, &ok, col, def);

    return ok;
}


bool xtok_get_double(XTokenizer* tok, int col, double def, double* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetDouble(tok, &ok, col, def);

    return ok;
}


bool xtok_get_float(XTokenizer* tok, int col, float def, float* dst)
{
    bool ok;

    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    *dst = X__GetDouble(tok, &ok, col, def);

    return ok;
}


bool xtok_get_string(XTokenizer* tok, int col, const char* def, char* dst, size_t size, bool strip)
{
    XTOK_ASSERT(tok);
    XTOK_ASSERT(dst);

    char buf[XTOK_MAX_COL_SIZE];
    char* token;
    bool ok = false;

    do
    {
        BREAK_IF((col < 0) || (tok->ntokens <= col));

        strncpy(buf, tok->tokens[col], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        token = strip ? X__Strip(buf) : buf;

        BREAK_IF(token[0] == '\0');

        strncpy(dst, token, size - 1);
        dst[size - 1] = '\0';
        ok = true;
    } while (0);

    if (! ok)
    {

        if (def == NULL)
            dst[0] = '\0';
        else
        {
            strncpy(dst, def, size - 1);
            dst[size - 1] = '\0';
        }
    }

    return ok;
}


bool xtok_get_bool(XTokenizer* tok, int col, bool def, bool* dst)
{
    char buf[XTOK_MAX_COL_SIZE];
    char* token;
    bool ok = false;
    int len;
    int i;
    bool value;

    do
    {
        BREAK_IF((col < 0) || (tok->ntokens <= col));

        strncpy(buf, tok->tokens[col], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        token = X__Strip(buf);

        BREAK_IF(token[0] == '\0');

        len = strlen(token);
        for (i = 0; i < len; i++)
        {
            token[i] = tolower((int)token[i]);
        }

        if (EQUAL_STR(token, "y")       ||
            EQUAL_STR(token, "yes")     ||
            EQUAL_STR(token, "true")    ||
            EQUAL_STR(token, "1"))
        {
            value = true;
        }

        else if (EQUAL_STR(token, "n")      ||
                 EQUAL_STR(token, "no")     ||
                 EQUAL_STR(token, "false")  ||
                 EQUAL_STR(token, "0"))
        {
            value = false;
        }

        else
        {
            break;
        }
        ok = true;
    } while (0);

    *dst = ok ? value : def;

    return ok;
}


static char* X__Strip(char* str)
{
    int len = strlen(str);

    if (! len)
        return str;

    int i;
    for (i = len - 1; i >= 0; i--)
    {
        if (! isspace((unsigned)str[i]))
            break;
    }
    str[i + 1] = '\0';
    len -= len - i;

    for (i = 0; i < len; i++)
    {
        if (! isspace((unsigned)str[i]))
            break;
    }
    return &str[i];
}


static uint32_t X__GetInt(XTokenizer* tok, bool* ok, int col, uint32_t def)
{
    char buf[XTOK_MAX_COL_SIZE];
    char* token;
    char* endptr;
    uint32_t value = 0;

    *ok = false;
    do
    {
        BREAK_IF((col < 0) || (tok->ntokens <= col));

        strncpy(buf, tok->tokens[col], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        token = X__Strip(buf);
        BREAK_IF(token[0] == '\0');

        value = strtoul(token, &endptr, 0);
        BREAK_IF(*endptr != '\0');

        *ok = true;

    } while (0);

    if (! *ok)
        value = def;

    return value;
}


static double X__GetDouble(XTokenizer* tok, bool* ok, int col, double def)
{
    char buf[XTOK_MAX_COL_SIZE];
    char* token;
    char* endptr;
    double value = 0;

    *ok = false;
    do
    {
        BREAK_IF((col < 0) || (tok->ntokens <= col));

        strncpy(buf, tok->tokens[col], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        token = X__Strip(buf);
        BREAK_IF(token[0] == '\0');

        value = strtod(token, &endptr);
        BREAK_IF(*endptr != '\0');

        *ok = true;

    } while (0);

    if (! *ok)
        value = def;

    return value;
}

#if 0
#include <assert.h>
#include <stdio.h>

int main(int argc, char const* argv[])
{
    XTokenizer tok;
    char row[] = "10, path.bmp  , 1.205, yes, no, true,";

    xtok_parse_row(&tok, row, ',');

    bool ok;
    int intv;
    unsigned uintv;
    int32_t int32v;
    uint32_t uint32v;
    bool boolv;
    double doublev;
    char strv[100];

    assert(xtok_num_tokens(&tok) == 7);

    ok = xtok_get_int(&tok, 0, 100, &intv);
    assert(ok && intv == 10);

    ok = xtok_get_string(&tok, 1, "UNKO", strv, sizeof(strv), true);
    assert(ok && EQUAL_STR(strv, "path.bmp"));

    ok = xtok_get_double(&tok, 2, 2.225, &doublev);
    printf("%f\n", doublev);

    ok = xtok_get_bool(&tok, 3, false, &boolv);
    assert(ok && (boolv == true));

    ok = xtok_get_bool(&tok, 4, true, &boolv);
    assert(ok && (boolv == false));

    ok = xtok_get_bool(&tok, 5, false, &boolv);
    assert(ok && (boolv == true));

    ok = xtok_get_int(&tok, 6, 9999, &intv);
    printf("%d\n", intv);
    assert((!ok) && (intv == 9999));

    return 0;
}
#endif
