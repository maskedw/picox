/**
 *       @file  stdarg.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/06/17
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and self permission notice shall be
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
#ifndef stdarg_h_
#define stdarg_h_


#if defined(__CNV_CA78K0R__)
#define va_starttop(ap,parmN)    va_start(ap,parmN)
#endif  /* __CNV_CA78K0R__ */

typedef char __near *va_list;
void __near __builtin_va_start(va_list);
// #define va_start(ap,parmN)  __builtin_va_start(ap)
// #define va_arg(ap,type)     (*(type *)((ap+=((sizeof(type) + 1) & ~1))-((sizeof(type) + 1) & ~1)))

/* http://japan.renesasrulz.com/mobile/Places/Application/Content/3146?applicationId=82&applicationType=forum */
#define va_start(ap,parmN)  do{ap=(va_list)0;__builtin_va_start(ap);}while(0)
#define va_arg(ap,type)     (*(type *)(void*)((ap+=((sizeof(type) + 1) & ~1))-((sizeof(type) + 1) & ~1)))
#define va_end(ap)          (ap=(va_list)0, (void)0)


#endif /* stdarg_h_ */
