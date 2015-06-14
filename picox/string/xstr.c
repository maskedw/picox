/**
 *       @file  xstr.c
 *      @brief
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


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "xstr.h"


static inline bool X__IsSkip(char c, const char* skip_chars);
static char* X__LStrip(char* str, int len, const char* strip_chars);
static char* X__RStrip(char* str, int len, const char* strip_chars);
static uint32_t X__GetInt(const char* str, bool* ok, uint32_t def);
static double X__GetDouble(const char* str, bool* ok, double def);


bool xstr_equal(const char* s1, const char* s2)
{
    return (strcmp(s1, s2) == 0);
}


bool xstr_case_equal(const char* s1, const char* s2)
{
    return (xstr_casecmp(s1, s2) == 0);
}


int xstr_casecmp(const char *s1, const char *s2)
{
    const unsigned char* us1 = (const unsigned char *)s1;
    const unsigned char* us2 = (const unsigned char *)s2;

    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return 0;
    return (tolower(*us1) - tolower(*--us2));
}


int xstr_ncasecmp(const char *s1, const char *s2, size_t n)
{
    const unsigned char* us1 = (const unsigned char *)s1;
    const unsigned char* us2 = (const unsigned char *)s2;

    if (n != 0)
    {
        do
        {
            if (tolower(*us1) != tolower(*us2++))
                return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                break;
        } while (--n != 0);
    }
    return 0;
}



char* xstr_reverse(char* str)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
    return str;
}


char* xstr_strip(char* str, const char* space)
{
    const int len = strlen(str);
    char* ret = NULL;

    ret = X__LStrip(str, len, space);

    if (ret)
    {
        ret = X__RStrip(ret, len - (ret - str), space);
    }

    return ret;
}


char* xstr_lstrip(char* str, const char* space)
{
    const int len = strlen(str);

    return X__LStrip(str, len, space);
}


char* xstr_rstrip(char* str, const char* space)
{
    const int len = strlen(str);

    return X__RStrip(str, len, space);
}


bool xstr_get_int(const char* str, int def, int* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetInt(str, &ok, def);

    return ok;
}


bool xstr_get_uint(const char* str, unsigned def, unsigned* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetInt(str, &ok, def);

    return ok;
}


bool xstr_get_int32(const char* str, int32_t def, int32_t* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetInt(str, &ok, def);

    return ok;
}


bool xstr_get_uint32(const char* str, uint32_t def, uint32_t* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetInt(str, &ok, def);

    return ok;
}


bool xstr_get_double(const char* str, double def, double* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetDouble(str, &ok, def);

    return ok;
}


bool xstr_get_float(const char* str, float def, float* dst)
{
    bool ok;

    XSTR_ASSERT(dst);
    *dst = X__GetDouble(str, &ok, def);

    return ok;
}


bool xstr_get_bool(const char* str, bool def, bool* dst)
{
    bool value;
    bool ok = true;

    XSTR_ASSERT(dst);

    if (xstr_case_equal(str, "y")       ||
        xstr_case_equal(str, "yes")     ||
        xstr_case_equal(str, "true")    ||
        xstr_case_equal(str, "1"))
    {
        value = true;
    }
    else if (xstr_case_equal(str, "n")      ||
             xstr_case_equal(str, "no")     ||
             xstr_case_equal(str, "false")  ||
             xstr_case_equal(str, "0"))
    {
        value = false;
    }
    else
    {
        ok = false;
    }

    *dst = ok ? value : def;

    return ok;
}



static inline bool X__IsSkip(char c, const char* skip_chars)
{
    for (;;)
    {
        if (*skip_chars == '\0')
            return false;

        if (c == (*skip_chars++))
            return true;
    }
}


static char* X__LStrip(char* str, int len, const char* strip_chars)
{
    int i;
    char* ret = str;

    if ((len == 0) || (str == NULL))
        return NULL;

    if (strip_chars == NULL)
    {
        for (i = 0; i < len; i++)
        {
            if (! isspace((unsigned)str[i]))
                break;
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            if (! X__IsSkip(str[i], strip_chars))
                break;
        }
    }

    ret = &str[i];

    return ret[0] ? ret : NULL;
}


static char* X__RStrip(char* str, int len, const char* strip_chars)
{
    int i;
    char* ret = str;

    if ((len == 0) || (str == NULL))
        return NULL;

    if (strip_chars == NULL)
    {
        for (i = len - 1; i >= 0; i--)
        {
            if (! isspace((unsigned)str[i]))
                break;
        }
    }
    else
    {
        for (i = len - 1; i >= 0; i--)
        {
            if (! X__IsSkip(str[i], strip_chars))
                break;
        }
    }

    str[i + 1] = '\0';

    return ret[0] ? ret : NULL;
}


static uint32_t X__GetInt(const char* str, bool* ok, uint32_t def)
{
    char* endptr;
    uint32_t value;
    *ok = false;

    value = strtoul(str, &endptr, 0);
    *ok = (endptr[0] == '\0');

    if (! *ok)
        value = def;

    return value;
}


static double X__GetDouble(const char* str, bool* ok, double def)
{
    char* endptr;
    double value;
    *ok = false;

    value = strtod(str, &endptr);
    *ok = (endptr[0] == '\0');

    if (! *ok)
        value = def;

    return value;
}
