/**
 *       @file  xstring.c
 *      @brief  標準Cライブラリstring.hの拡張実装
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


#include <picox/core/xcore.h>


static bool X__ToInt(const char* s, uint32_t* dst, bool negativable);
static bool X__IsSkip(char c, const char* skip_chars);
static char* X__StripLeft(char* str, int len, const char* strip_chars);
static char* X__StripRight(char* str, int len, const char* strip_chars);


bool x_strequal(const char* s1, const char* s2)
{
    return (strcmp(s1, s2) == 0);
}


bool x_strcaseequal(const char* s1, const char* s2)
{
    return (x_strcasecmp(s1, s2) == 0);
}


int x_strcasecmp(const char *s1, const char *s2)
{
    while (tolower((int)*s1) == tolower((int)*s2++))
        if (*s1++ == '\0')
            return 0;
    return (tolower((int)*s1) - tolower((int)*--s2));
}


int x_strncasecmp(const char *s1, const char *s2, size_t n)
{
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


char* x_strnstr(const char* s1, const char* s2, size_t n)
{
    char sc2, sc1;
    size_t len;

    if ((sc2 = *s2++) != '\0')
    {
        len = strlen(s2);
        do
        {
            do
            {
                if ((n-- < 1) || ((sc1 = *s1++) == '\0'))
                    return NULL;
            } while (sc1 != sc2);
            if (len > n)
                return NULL;
        } while (strncmp(s1, s2, len) != 0);
        s1--;
    }
    return (char *)s1;
}


char* x_strncasestr(const char* s1, const char* s2, size_t n)
{
    char sc2, sc1;
    size_t len;

    if ((sc2 = *s2++) != '\0')
    {
        len = strlen(s2);
        do
        {
            do
            {
                if ((n-- < 1) || ((sc1 = *s1++) == '\0'))
                    return NULL;
            } while (sc1 != sc2);
            if (len > n)
                return NULL;
        } while (x_strncasecmp(s1, s2, len) != 0);
        s1--;
    }
    return (char *)s1;
}



// http://stackoverflow.com/questions/211535/fastest-way-to-do-a-case-insensitive-substring-search-in-c-c
char* x_strcasestr(const char* s1, const char* s2)
{
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


char* x_strdup(const char* str)
{
    return x_strdup2(str, x_malloc);
}


char* x_strdup2(const char* str, XMallocFunc malloc_func)
{
    return x_strndup2(str, SIZE_MAX, malloc_func);
}


char* x_strndup(const char* str, size_t n)
{
    return x_strndup2(str, n, x_malloc);
}


char* x_strndup2(const char* str, size_t n, XMallocFunc malloc_func)
{
    const size_t slen = strlen(str);
    const size_t len = slen > n ? n : slen;

    char* p = malloc_func(len + 1);
    if (!p)
        return NULL;

    memcpy(p, str, len);
    p[len] = '\0';

    return p;
}


char* x_strreverse(char* str)
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


char* x_strstrip(char* str, const char* space)
{
    const int len = strlen(str);
    char* ret = NULL;

    ret = X__StripLeft(str, len, space);

    if (ret)
        ret = X__StripRight(ret, len - (ret - str), space);

    return ret;
}


char* x_strlstrip(char* str, const char* space)
{
    const int len = strlen(str);

    return X__StripLeft(str, len, space);
}


char* x_strrstrip(char* str, const char* space)
{
    const int len = strlen(str);

    return X__StripRight(str, len, space);
}


int32_t x_strtoint32(const char* str, int32_t def, bool* ok)
{
    bool sub;
    uint32_t dst;
    int32_t ret;
    if (! ok) ok = &sub;

    *ok = X__ToInt(str, &dst, true);
    ret = *ok ? (int32_t)dst : def;

    return ret;
}


uint32_t x_strtouint32(const char* str, uint32_t def, bool* ok)
{
    bool sub;
    uint32_t dst;
    uint32_t ret;

    if (! ok) ok = &sub;

    *ok = X__ToInt(str, &dst, false);
    ret = *ok ? dst : def;

    return ret;
}


float x_strtofloat(const char* str, float def, bool* ok)
{
#if X_CONF_HAS_C99_MATH == 0
    return x_strtodouble(str, def, ok);
#else

    bool sub;
    char* endptr;
    float v;
    float ret;

    if (! ok) ok = &sub;
    v  = strtof(str, &endptr);
    *ok = (endptr[0] == '\0');
    ret = *ok ? v : def;

    return ret;
#endif

}


double x_strtodouble(const char* str, double def, bool* ok)
{
    bool sub;
    char* endptr;
    double v;
    double ret;

    if (! ok) ok = &sub;
    v  = strtod(str, &endptr);
    *ok = (endptr[0] == '\0');
    ret = *ok ? v : def;

    return ret;
}


bool x_strtobool(const char* str, bool def, bool* ok)
{
    bool v;

    X_ASSIGN_NOT_NULL(ok, true);
    if (x_strcaseequal(str, "y")       ||
        x_strcaseequal(str, "yes")     ||
        x_strcaseequal(str, "true")    ||
        x_strcaseequal(str, "1"))
    {
        v = true;
    }
    else if (x_strcaseequal(str, "n")      ||
             x_strcaseequal(str, "no")     ||
             x_strcaseequal(str, "false")  ||
             x_strcaseequal(str, "0"))
    {
        v = false;
    }
    else
    {
        v = def;
        X_ASSIGN_NOT_NULL(ok, false);
    }

    return v;
}


char* x_strrpbrk(const char* str, const char* accept)
{
    const char* top;
    const char* p;
    const char* c;
    if (str[0] == '\0')
        return NULL;

    top = str;
    p = str + strlen(str) - 1;

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


char* x_strcasepbrk(const char* str, const char* accept)
{
    const char* p = str;
    const char* c;

    if (str[0] == '\0')
        return NULL;

    for (;;)
    {
        for (c = accept; *c; c++)
        {
            if (toupper((int)*p) == toupper((int)*c))
                break;
        }

        if (*c)
            break;
        if (!*p)
            break;

        ++p;
    }

    if (*c == '\0')
        p = NULL;

    return (char*)p;
}


char* x_strcaserpbrk(const char* str, const char* accept)
{
    const char* top;
    const char* p;
    const char* c;
    if (str[0] == '\0')
        return NULL;

    top = str;
    p = str + strlen(str) - 1;

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


char* x_strtolower(char* str)
{
    char* p = str;
    while (*p)
    {
        *p = (char)(tolower((int)*p));
        p++;
    }

    return str;
}


char* x_strtoupper(char* str)
{
    char* p = str;
    while (*p)
    {
        *p = (char)(toupper((int)*p));
        p++;
    }

    return str;
}


size_t x_strlcpy(char* dst, const char* src, size_t n)
{
    size_t len, size;
    if (n == 0)
        return 0;

    len = strlen(src);
    size = (n > len) ? len : n - 1;
    memcpy(dst, src, size);
    dst[size] = '\0';

    return len;
}


size_t x_strlcat(char * dst, const char * src, size_t dsize)
{
    const char *odst = dst;
    const char *osrc = src;
    size_t n = dsize;
    size_t dlen;

    while (n-- != 0 && *dst != '\0')
        dst++;
    dlen = dst - odst;
    n = dsize - dlen;

    if (n-- == 0)
        return dlen + strlen(src);
    while (*src != '\0')
    {
        if (n != 0)
        {
            *dst++ = *src;
            n--;
        }
        src++;
    }
    *dst = '\0';

    return dlen + (src - osrc);
}


size_t x_strcountequal(const char* a, const char* b)
{
    size_t n = 0;
    while (*a == *b++)
    {
        if (*a++ == '\0')
            break;
        ++n;
    }

    return n;
}


size_t x_strcountcaseequal(const char* a, const char* b)
{
    size_t n = 0;
    while (tolower((int)*a) == tolower((int)(*b++)))
    {
        if (*a++ == '\0')
            break;
        ++n;
    }

    return n;
}


void* x_memmem(const void* p1, size_t n1, const void* p2, size_t n2)
{
    const char* cur;
    const char* last;
    const char* s1 = p1;
    const char* s2 = p2;

    if (n2 == 0)
        return (char*)s1;

    if (n1 < n2)
        return NULL;

    if (n2 == 1)
        return memchr(p1, *s2, n1);

    last = s1 + n1 - n2;
    for (cur = s1; cur <= last; cur++)
        if ((cur[0] == s2[0]) && (memcmp(cur, s2, n2) == 0))
            return (char*)cur;

    return NULL;
}


void* x_memrchr(const void* p, int c, size_t n)
{
    const uint8_t* cp;

    if (n != 0)
    {
        cp = (const uint8_t*)p  + n;
        do
        {
            if (*(--cp) == (uint8_t)c)
                return (void*)cp;
        } while (--n != 0);
    }
    return NULL;
}


void x_memswap(void *a, void *b, size_t size)
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


void x_memreverse(void *p, size_t size, size_t n)
{
    char* l;
    char* r;

    l = (char*)p;
    r = (char*)p + ((n-1) * size);
    for ( ; l < r; l = l+size, r = r-size)
        x_memswap( l, r, size );
}


void x_memrrot(void *p, size_t shift, size_t size, size_t n)
{
    x_memreverse(p,                      size, n);          // 012345678 -> 876543210
    x_memreverse(p,                      size, shift);      // 876543210 -> 678543210
    x_memreverse((char*)p+(size*shift),  size, n-shift);    // 678543210 -> 678012345
}


void x_memlrot(void *p, size_t shift, size_t size, size_t n)
{
    x_memreverse(p,                      size, shift);      // 012345678 -> 210345678
    x_memreverse((char*)p+(size*shift),  size, n-shift);    // 210345678 -> 210876543
    x_memreverse(p,                      size, n);          // 210876543 -> 345678012
}


void x_memblt(void* dst, const void* src,
              size_t width, size_t height,
              size_t dstride, size_t sstride)
{
    char* cdst = (char*)dst;
    const char* csrc = (const char*)src;

    size_t y;
    for (y = 0; y < height; ++y)
    {
        memcpy(cdst, csrc, width);
        cdst += dstride;
        csrc += sstride;
    }
}


bool x_memequal(const void* p1, const void* p2, size_t n)
{
    return memcmp(p1, p2, n) == 0;
}


char* x_stpcpy(char* dst, const char* src)
{
    for (; (*dst = *src); ++src, ++dst);
    return dst;
}


char* x_stprcpy(char* dst, const char* src)
{
    const size_t len = strlen(src);
    const char* p;
    if (len == 0)
    {
        *dst = '\0';
        return dst;
    }

    p = src + len - 1;
    do
    {
        *dst++ = *p--;
    } while (p >= src);

    *dst = '\0';
    return dst;
}


char* x_stpncpy(char* dst, const char* src, size_t n)
{
    for (; n--; dst++, src++)
    {
        if (!(*dst = *src))
        {
            char *ret = dst;
            while (n--)
                *++dst = '\0';
            return ret;
        }
    }
    *dst = '\0';
    return dst;
}


char* x_stpncpy2(char* dst, const char* src, size_t n)
{
    for (; n--; dst++, src++)
    {
        if (!(*dst = *src))
            return dst;
    }
    *dst = '\0';
    return dst;
}


char* x_strncpy2(char* dst, const char* src, size_t n)
{
    x_stpncpy2(dst, src, n);

    return dst;
}


char* x_strchrnul(const char* s, int c)
{
    const char ch = (char)c;
    for (;; ++s) {
        if ((*s == ch) || (*s == '\0'))
            return (char *)s;
    }

    /* NOTREACHED */
    return NULL;
}


size_t x_strnlen(const char* s, size_t n)
{
    size_t len;

    for (len = 0; len < n; ++len, ++s)
    {
        if (!*s)
            break;
    }
    return len;
}


XOpenMode x_strtomode(const char* strmode)
{
    const size_t len = strlen(strmode);
    XOpenMode mode = X_OPEN_MODE_UNKNOWN;
    char buf[4];

    if (len == 0 || len > 3)
        return mode;

    strcpy(buf, strmode);

    if (buf[len - 1] == 'b')
        buf[len - 1] = '\0';

    if (x_strequal(buf, "r"))
        mode = X_OPEN_MODE_READ;
    else if (x_strequal(buf, "r+"))
        mode = X_OPEN_MODE_READ_PLUS;
    else if (x_strequal(buf, "w"))
        mode = X_OPEN_MODE_WRITE;
    else if (x_strequal(buf, "w+"))
        mode = X_OPEN_MODE_WRITE_PLUS;
    else if (x_strequal(buf, "a"))
        mode = X_OPEN_MODE_APPEND;
    else if (x_strequal(buf, "a+"))
        mode = X_OPEN_MODE_APPEND_PLUS;

    return mode;
}


char* x_strskipchr(const char* s, int c)
{
    while (*s == c)
        ++s;
    return (char*)s;
}


char* x_strreplace(char* dst, size_t size, size_t len, size_t dn, const char* src, size_t sn)
{
    if (dn == sn)
    {
        memcpy(dst, src, sn);
        return dst;
    }

    if (sn > dn)
    {
        if (sn - dn + len >= size)
            return NULL;
    }

    if (sn == 0)
        return dst;

    memmove(dst + sn, dst + dn, len - dn + 1);
    memcpy(dst, src, sn);

    return dst;
}


void x_memrandom(void* p, size_t n)
{
    uint8_t* up = p;
    size_t i;

    for (i = 0; i < n; i++)
        *up++ = (uint8_t)x_rand();
}


void x_memrandom_alpha(void* p, size_t n)
{
    const char alpha[] = "abcdefghijklmnopqrstuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uint8_t* up = p;
    size_t i;

    for (i = 0; i < n; i++)
        *up++ = alpha[x_rand() % sizeof(alpha)];
}


const char* x_btos(bool cond)
{
    return cond ? "true" : "false";
}


const char* x_strerror(XError err)
{
    const char* ret = NULL;
    switch (err)
    {
        case X_ERR_NONE:                ret = "ERR_NONE";           break;
        case X_ERR_IO:                  ret = "ERR_IO";             break;
        case X_ERR_INVALID:             ret = "ERR_INVALID";        break;
        case X_ERR_TIMED_OUT:           ret = "ERR_TIMED_OUT";      break;
        case X_ERR_BUSY:                ret = "ERR_BUSY";           break;
        case X_ERR_AGAIN:               ret = "ERR_AGAIN";          break;
        case X_ERR_CANCELED:            ret = "ERR_CANCELED";       break;
        case X_ERR_NO_MEMORY:           ret = "ERR_NO_MEMORY";      break;
        case X_ERR_EXIST:               ret = "ERR_EXIST";          break;
        case X_ERR_NOT_READY:           ret = "ERR_NOT_READY";      break;
        case X_ERR_ACCESS:              ret = "ERR_ACCESS";         break;
        case X_ERR_NO_ENTRY:            ret = "ERR_NO_ENTRY";       break;
        case X_ERR_NOT_SUPPORTED:       ret = "ERR_NOT_SUPPORTED";  break;
        case X_ERR_DISCONNECTED:        ret = "ERR_DISCONNECTED";   break;
        case X_ERR_INPROGRESS:          ret = "ERR_INPROGRESS";     break;
        case X_ERR_PROTOCOL:            ret = "ERR_PROTOCOL";       break;
        case X_ERR_MANY:                ret = "ERR_MANY";           break;
        case X_ERR_RANGE:               ret = "ERR_RANGE";          break;
        case X_ERR_BROKEN:              ret = "ERR_BROKEN";         break;
        case X_ERR_NAME_TOO_LONG:       ret = "ERR_NAME_TOO_LONG";  break;
        case X_ERR_INVALID_NAME:        ret = "ERR_INVALID_NAME";   break;
        case X_ERR_IS_DIRECTORY:        ret = "ERR_IS_DIRECTORY";   break;
        case X_ERR_NOT_DIRECTORY:       ret = "ERR_NOT_DIRECTORY";  break;
        case X_ERR_NOT_EMPTY:           ret = "ERR_NOT_EMPTY";      break;
        case X_ERR_NO_SPACE:            ret = "ERR_NO_SPACE";       break;
        case X_ERR_INTERNAL:            ret = "ERR_INTERNAL";       break;
        case X_ERR_OTHER:               ret = "ERR_OTHER";          break;
        default:                        ret = "ERR_UNKNOWN";        break;
    }

    return ret;
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
    int base;
    uint32_t cutoff;
    uint32_t acc;
    int cutlimit;

    /* 先頭の空白はすっ飛ばす。*/
    do { c = *s++; } while (isspace(c));

    /* 符号の確認 */
    if (c == '-')
    {
        minus = true;
        sign = true;
        c = *s++;
    }
    else if (c == '+') {
        sign = true;
        c = *s++;
    }

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

    cutoff = (minus ? -((uint32_t)INT32_MIN) : negativable ? INT32_MAX : UINT32_MAX);
    cutlimit = (int)(cutoff % base);
    cutoff /= base;

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
