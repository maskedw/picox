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


#include <picox/string/xstr.h>


static bool X__ToInt(const char* s, uint32_t* dst, bool negativable);
static bool X__IsSkip(char c, const char* skip_chars);
static char* X__StripLeft(char* str, int len, const char* strip_chars);
static char* X__StripRight(char* str, int len, const char* strip_chars);



bool xstr_equal(const char* s1, const char* s2)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    return (strcmp(s1, s2) == 0);
}


bool xstr_case_equal(const char* s1, const char* s2)
{
    return (xstr_case_compare(s1, s2) == 0);
}


int xstr_compare(const char* s1, const char* s2)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    return strcmp(s1, s2);
}


int xstr_case_compare(const char *s1, const char *s2)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    while (tolower((int)*s1) == tolower((int)*s2++))
        if (*s1++ == '\0')
            return 0;
    return (tolower((int)*s1) - tolower((int)*--s2));
}


int xstr_ncase_compare(const char *s1, const char *s2, size_t n)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    if (n != 0)
    {
        do
        {
            if (tolower((int)*s1) != tolower((int)*s2++))
                return (tolower((int)*s1) - tolower((int)*--s2));
            if (*s1++ == '\0')
                break;
        } while (--n != 0);
    }
    return 0;
}


char* xstr_search_substring(const char* s1, const char* s2)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    return strstr(s1, s2);
}


// http://stackoverflow.com/questions/211535/fastest-way-to-do-a-case-insensitive-substring-search-in-c-c
char* xstr_case_search_substring(const char* s1, const char* s2)
{
    X_ASSERT(s1);
    X_ASSERT(s2);

    char*cp = (char*)s1;
    char*as1, *as2;

    if ( !*s2 )
        return (char*)s1;

    while (*cp)
    {
        as1 = cp;
        as2 = (char*) s2;

        while (*as1 && *as2 && (isalpha((int)*as1) &&
               isalpha((int)*as2)) ? !(toupper((int)*as1) - toupper((int)*as2)) : !(*as1 - *as2))
            ++as1, ++as2;

        if (!*as2)
            return cp;
        ++cp;
    }

    return NULL;
}


char* xstr_duplicate(const char* str)
{
    return xstr_duplicate2(str, X_MALLOC);
}


char* xstr_duplicate2(const char* str, void* (*malloc_func)(size_t))
{
    return xstr_nduplicate2(str, SIZE_MAX, malloc_func);
}


char* xstr_nduplicate(const char* str, size_t n)
{
    return xstr_nduplicate2(str, n, X_MALLOC);
}


char* xstr_nduplicate2(const char* str, size_t n, void* (*malloc_func)(size_t))
{
    X_ASSERT(str);
    X_ASSERT(malloc_func);

    const size_t slen = strlen(str);
    const size_t len = slen > n ? n : slen;

    char* p = malloc_func(len + 1);
    if (!p)
        return NULL;

    memcpy(p, str, len);
    p[len] = '\0';

    return p;
}


char* xstr_reverse(char* str)
{
    X_ASSERT(str);
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
    X_ASSERT(str);
    const int len = strlen(str);
    char* ret = NULL;

    ret = X__StripLeft(str, len, space);

    if (ret)
        ret = X__StripRight(ret, len - (ret - str), space);

    return ret;
}


char* xstr_strip_left(char* str, const char* space)
{
    X_ASSERT(str);
    const int len = strlen(str);

    return X__StripLeft(str, len, space);
}


char* xstr_strip_right(char* str, const char* space)
{
    X_ASSERT(str);
    const int len = strlen(str);

    return X__StripRight(str, len, space);
}


int32_t xstr_to_int32(const char* str, int32_t def, bool* ok)
{
    X_ASSERT(str);

    bool sub;
    if (! ok) ok = &sub;

    uint32_t dst;
    *ok = X__ToInt(str, &dst, true);
    const int32_t ret = *ok ? (int32_t)dst : def;

    return ret;
}


uint32_t xstr_to_uint32(const char* str, uint32_t def, bool* ok)
{
    X_ASSERT(str);

    bool sub;
    if (! ok) ok = &sub;

    uint32_t dst;
    *ok = X__ToInt(str, &dst, false);
    const uint32_t ret = *ok ? dst : def;

    return ret;
}


float xstr_to_float(const char* str, float def, bool* ok)
{
#ifndef X_CONF_HAS_C99_MATH
    return xstr_to_double(str, def, ok);
#else
    X_ASSERT(str);

    bool sub;
    if (! ok) ok = &sub;

    char* endptr;
    const float v  = strtof(str, &endptr);
    *ok = (endptr[0] == '\0');
    const float ret = *ok ? v : def;

    return ret;
#endif

}


double xstr_to_double(const char* str, double def, bool* ok)
{
    X_ASSERT(str);

    bool sub;
    if (! ok) ok = &sub;

    char* endptr;
    const double v  = strtod(str, &endptr);
    *ok = (endptr[0] == '\0');
    const double ret = *ok ? v : def;

    return ret;
}


bool xstr_to_bool(const char* str, bool def, bool* ok)
{
    X_ASSERT(str);

    bool v;
    bool sub;
    if (! ok) ok = &sub;

    *ok = true;
    if (xstr_case_equal(str, "y")       ||
        xstr_case_equal(str, "yes")     ||
        xstr_case_equal(str, "true")    ||
        xstr_case_equal(str, "1"))
    {
        v = true;
    }
    else if (xstr_case_equal(str, "n")      ||
             xstr_case_equal(str, "no")     ||
             xstr_case_equal(str, "false")  ||
             xstr_case_equal(str, "0"))
    {
        v = false;
    }
    else
        *ok = false;

    const bool ret = *ok ? v : def;

    return ret;
}


char* xstr_pbrk(const char* str, const char* accept)
{
    X_ASSERT(str);
    X_ASSERT(accept);

    return strpbrk(str, accept);
}


char* xstr_rpbrk(const char* str, const char* accept)
{
    X_ASSERT(str);
    X_ASSERT(accept);

    if (str[0] == '\0')
        return NULL;

    const char* top = str;
    const char* p = str + strlen(str) - 1;
    const char* c;

    for (;;)
    {
        for (c = accept; *c; c++)
        {
            if (*p == *c)
                break;
        }
        if (*c)
            break;
        if (p == top)
            break;

        p--;
    }

    if (*c == '\0')
        p = NULL;

    return (char*)p;
}


char* xstr_case_pbrk(const char* str, const char* accept)
{
    X_ASSERT(str);
    X_ASSERT(accept);


    if (str[0] == '\0')
        return NULL;

    const char* c;
    while (*str)
    {
        for (c = accept; *c; c++)
        {
            if (toupper((int)*str) == toupper((int)*c))
                break;
        }

        if (*c)
            break;
        str++;
    }

    if (*c == '\0')
        str = NULL;

    return (char*)str;
}


char* xstr_case_rpbrk(const char* str, const char* accept)
{
    X_ASSERT(str);
    X_ASSERT(accept);

    if (str[0] == '\0')
        return NULL;

    const char* top = str;
    const char* p = str + strlen(str) - 1;
    const char* c;

    for (;;)
    {
        for (c = accept; *c; c++)
        {
            if (toupper((int)*p) == toupper((int)*c))
                break;
        }
        if (*c)
            break;
        if (p == top)
            break;

        p--;
    }

    if (*c == '\0')
        p = NULL;

    return (char*)p;
}


char* xstr_to_lower(char* str)
{
    X_ASSERT(str);

    char* p = str;
    while (*p)
    {
        *p = tolower((int)*p);
        p++;
    }

    return str;
}


char* xstr_to_upper(char* str)
{
    X_ASSERT(str);

    char* p = str;
    while (*p)
    {
        *p = toupper((int)*p);
        p++;
    }

    return str;
}


static bool X__IsSkip(char c, const char* skip_chars)
{
    for (;;)
    {
        if (*skip_chars == '\0')
            return false;

        if (c == (*skip_chars++))
            return true;
    }
}


static char* X__StripLeft(char* str, int len, const char* strip_chars)
{
    int i;
    char* ret = str;

    X_ASSERT(str);
    if (len == 0)
        return str;

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


static char* X__StripRight(char* str, int len, const char* strip_chars)
{
    int i;
    char* ret = str;

    X_ASSERT(str);
    if (len == 0)
        return str;

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


/*
 * strtul()は便利だが、やや曖昧な変換を行うのでもう少し厳密な変換を行う。
 *
 * + 先頭の1個以上の空白は認める。
 * + 10進数の時のみ、+-の符号を認める。
 * + 2, 10, 16進数に対応する。2進数は0[bB], 16進数は0[xX]を先頭につける。
 * + strtolは"0x"だけを渡すと、0の部分だけを10進数として解釈するが、これはエラーとして扱うことにする。
 */
static bool X__ToInt(const char* s, uint32_t* dst, bool negativable)
{
    int c;
    bool minus = false;
    bool sign = false;

    /* 先頭の空白はすっ飛ばす。*/
    do { c = *s++; } while (isspace(c));

    /* 符号の確認 */
    if (c == '-')
    {
        minus = true;
        sign = true;
        c = *s++;
    }
    else if (c == '+')
    {
        sign = true;
        c = *s++;
    }

    int base;
    /* 基数の確認。 2, 10, 16進数に対応する。 */
    if (c == '0' && (*s == 'x' || *s == 'X'))
    {
        if (sign)
            return false;

        c = s[1];
        s += 2;
        base = 16;
    }
    else if (c == '0' && (*s == 'b' || *s == 'B'))
    {
        if (sign)
            return false;

        c = s[1];
        s += 2;
        base = 2;
    }
    else
    {
        if (minus && (!negativable))
            return false;
        base = 10;
    }

    if (c == '\0')
        return false;

    uint32_t cutoff = (minus ? -((uint32_t)INT32_MIN) : negativable ? INT32_MAX : UINT32_MAX);
    const int cutlimit = cutoff % base;
    cutoff /= base;

    uint32_t acc;
    for (acc = 0;; c = *s++)
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (c >= base)
            return false;

        if ((acc > cutoff) || ((acc == cutoff) && (c > cutlimit)))
            return false;

        acc *= base;
        acc += c;
    }

    if (c != '\0')
        return false;

    if (minus)
        acc = -acc;

    *dst = acc;

    return true;
}
