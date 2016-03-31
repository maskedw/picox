/**
 *       @file  xdynamic_string.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/03
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


#include <sds.h>
#include <picox/string/xdynamic_string.h>


struct XDynamicString
{
    sds body;
};


XDynamicString* xdstr_create(const char* src)
{
    sds const ret = sdsnew(src);
    return (XDynamicString*)ret;
}


XDynamicString* xdstr_create_length(const char* src, size_t len)
{
    sds const ret = sdsnewlen(src, len);
    return (XDynamicString*)ret;
}


XDynamicString* xdstr_create_empty(void)
{
    sds const ret = sdsempty();
    return (XDynamicString*)ret;
}


void xdstr_destroy(XDynamicString* self)
{
    sdsfree((sds)self);
}


XDynamicString* xdstr_clone(const XDynamicString* self)
{
    X_ASSERT(self);
    return (XDynamicString*)(sdsdup((const sds)self));
}


XDynamicString* xdstr_cat(XDynamicString* self, const char* str)
{
    X_ASSERT(self);
    if (str == NULL)
        return self;
    return (XDynamicString*)(sdscat((sds)self, str));
}


XDynamicString* xdstr_cat_n(XDynamicString* self, const char* str, size_t len)
{
    X_ASSERT(self);
    if (str == NULL)
        return self;
    return (XDynamicString*)(sdscatlen((sds)self, str, len));
}


XDynamicString* xdstr_cat_printf(XDynamicString* self, const char *fmt, ...)
{
    X_ASSERT(self);
    if (fmt == NULL)
        return self;
    va_list args;
    va_start(args, fmt);
    XDynamicString* const ret  = xdstr_cat_vprintf(self, fmt, args);
    va_end(args);

    return ret;
}


XDynamicString* xdstr_cat_vprintf(XDynamicString* self, const char *fmt, va_list args)
{
    X_ASSERT(self);
    if (fmt == NULL)
        return self;
    return (XDynamicString*)(sdscatvprintf((sds)self, fmt, args));
}


XDynamicString* xdstr_copy(XDynamicString* self, const char* str)
{
    X_ASSERT(self);
    if (str == NULL)
    {
        ((char*)self)[0] = '\0';
        sdsupdatelen((sds)self);
        return self;
    }
    return (XDynamicString*)(sdscpy((sds)self, str));
}


XDynamicString* xdstr_copy_n(XDynamicString* self, const char* str, size_t len)
{
    X_ASSERT(self);
    if ((str == NULL) || (len == 0))
    {
        ((char*)self)[0] = '\0';
        sdsupdatelen((sds)self);
        return self;
    }
    return (XDynamicString*)(sdscpylen((sds)self, str, len));
}


size_t xdstr_capacity(const XDynamicString* self)
{
    X_ASSERT(self);
    return sdslen((const sds)self) + sdsavail((const sds)self);
}


void xdstr_trim(XDynamicString* self, const char* char_set)
{
    X_ASSERT(self);
    if (char_set != NULL)
        sdstrim((sds)self, char_set);
}


void xdstr_range(XDynamicString* self, int first, int last)
{
    X_ASSERT(self);
    sdsrange((sds)self, first, last);
}


void xdstr_to_lower(XDynamicString* self)
{
    X_ASSERT(self);
    sdstolower((sds)self);
}


void xdstr_to_upper(XDynamicString* self)
{
    X_ASSERT(self);
    sdstoupper((sds)self);
}


XDynamicString* xdstr_shrink_to_fit(XDynamicString* self)
{
    X_ASSERT(self);
    return (XDynamicString*)(sdsRemoveFreeSpace((sds)self));
}


XDynamicString* xdstr_reserve(XDynamicString* self, size_t size)
{
    X_ASSERT(self);

    const size_t curlen = sdslen((sds)self);
    if (curlen >= size)
        return self;
    sds new_str = sdsMakeFitRoomFor((sds)self, size - curlen);
    if (!new_str)
        return NULL;
    return (XDynamicString*)new_str;
}


const char* xdstr_c_str(const XDynamicString* self)
{
    X_ASSERT(self);
    return (const char*)self;
}


size_t xdstr_length(const XDynamicString* self)
{
    X_ASSERT(self);
    return sdslen((const sds)self);
}
