/**
 *       @file  inttypes.h
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
#ifndef inttypes_h_
#define inttypes_h_


#define __STRINGIFY(a) #a

/* 8-bit types */
#define __PRI8(x) __STRINGIFY(x)
#define __SCN8(x) __STRINGIFY(hh##x)


#define PRId8		__PRI8(d)
#define PRIi8		__PRI8(i)
#define PRIo8		__PRI8(o)
#define PRIu8		__PRI8(u)
#define PRIx8		__PRI8(x)
#define PRIX8		__PRI8(X)

#define SCNd8		__SCN8(d)
#define SCNi8		__SCN8(i)
#define SCNo8		__SCN8(o)
#define SCNu8		__SCN8(u)
#define SCNx8		__SCN8(x)


#define PRIdLEAST8	__PRI8(d)
#define PRIiLEAST8	__PRI8(i)
#define PRIoLEAST8	__PRI8(o)
#define PRIuLEAST8	__PRI8(u)
#define PRIxLEAST8	__PRI8(x)
#define PRIXLEAST8	__PRI8(X)

#define SCNdLEAST8	__SCN8(d)
#define SCNiLEAST8	__SCN8(i)
#define SCNoLEAST8	__SCN8(o)
#define SCNuLEAST8	__SCN8(u)
#define SCNxLEAST8	__SCN8(x)


#define PRIdFAST8	__PRI8(d)
#define PRIiFAST8	__PRI8(i)
#define PRIoFAST8	__PRI8(o)
#define PRIuFAST8	__PRI8(u)
#define PRIxFAST8	__PRI8(x)
#define PRIXFAST8	__PRI8(X)

#define SCNdFAST8	__SCN8(d)
#define SCNiFAST8	__SCN8(i)
#define SCNoFAST8	__SCN8(o)
#define SCNuFAST8	__SCN8(u)
#define SCNxFAST8	__SCN8(x)

/* 16-bit types */
#define __PRI16(x) __STRINGIFY(x)
#define __SCN16(x) __STRINGIFY(h##x)


#define PRId16		__PRI16(d)
#define PRIi16		__PRI16(i)
#define PRIo16		__PRI16(o)
#define PRIu16		__PRI16(u)
#define PRIx16		__PRI16(x)
#define PRIX16		__PRI16(X)

#define SCNd16		__SCN16(d)
#define SCNi16		__SCN16(i)
#define SCNo16		__SCN16(o)
#define SCNu16		__SCN16(u)
#define SCNx16		__SCN16(x)


#define PRIdLEAST16	__PRI16(d)
#define PRIiLEAST16	__PRI16(i)
#define PRIoLEAST16	__PRI16(o)
#define PRIuLEAST16	__PRI16(u)
#define PRIxLEAST16	__PRI16(x)
#define PRIXLEAST16	__PRI16(X)

#define SCNdLEAST16	__SCN16(d)
#define SCNiLEAST16	__SCN16(i)
#define SCNoLEAST16	__SCN16(o)
#define SCNuLEAST16	__SCN16(u)
#define SCNxLEAST16	__SCN16(x)


#define PRIdFAST16	__PRI16(d)
#define PRIiFAST16	__PRI16(i)
#define PRIoFAST16	__PRI16(o)
#define PRIuFAST16	__PRI16(u)
#define PRIxFAST16	__PRI16(x)
#define PRIXFAST16	__PRI16(X)

#define SCNdFAST16	__SCN16(d)
#define SCNiFAST16	__SCN16(i)
#define SCNoFAST16	__SCN16(o)
#define SCNuFAST16	__SCN16(u)
#define SCNxFAST16	__SCN16(x)

#define __PRI32(x) __STRINGIFY(l##x)
#define __SCN32(x) __STRINGIFY(l##x)

#define PRId32		__PRI32(d)
#define PRIi32		__PRI32(i)
#define PRIo32		__PRI32(o)
#define PRIu32		__PRI32(u)
#define PRIx32		__PRI32(x)
#define PRIX32		__PRI32(X)

#define SCNd32		__SCN32(d)
#define SCNi32		__SCN32(i)
#define SCNo32		__SCN32(o)
#define SCNu32		__SCN32(u)
#define SCNx32		__SCN32(x)


#define PRIdLEAST32	__PRI32(d)
#define PRIiLEAST32	__PRI32(i)
#define PRIoLEAST32	__PRI32(o)
#define PRIuLEAST32	__PRI32(u)
#define PRIxLEAST32	__PRI32(x)
#define PRIXLEAST32	__PRI32(X)

#define SCNdLEAST32	__SCN32(d)
#define SCNiLEAST32	__SCN32(i)
#define SCNoLEAST32	__SCN32(o)
#define SCNuLEAST32	__SCN32(u)
#define SCNxLEAST32	__SCN32(x)


#define PRIdFAST32	__PRI32(d)
#define PRIiFAST32	__PRI32(i)
#define PRIoFAST32	__PRI32(o)
#define PRIuFAST32	__PRI32(u)
#define PRIxFAST32	__PRI32(x)
#define PRIXFAST32	__PRI32(X)

#define SCNdFAST32	__SCN32(d)
#define SCNiFAST32	__SCN32(i)
#define SCNoFAST32	__SCN32(o)
#define SCNuFAST32	__SCN32(u)
#define SCNxFAST32	__SCN32(x)


/* max-bit types */
#define __PRIMAX(x) __STRINGIFY(l##x)
#define __SCNMAX(x) __STRINGIFY(l##x)

#define PRIdMAX		__PRIMAX(d)
#define PRIiMAX		__PRIMAX(i)
#define PRIoMAX		__PRIMAX(o)
#define PRIuMAX		__PRIMAX(u)
#define PRIxMAX		__PRIMAX(x)
#define PRIXMAX		__PRIMAX(X)

#define SCNdMAX		__SCNMAX(d)
#define SCNiMAX		__SCNMAX(i)
#define SCNoMAX		__SCNMAX(o)
#define SCNuMAX		__SCNMAX(u)
#define SCNxMAX		__SCNMAX(x)

/* ptr types */
#define __PRIPTR(x) __STRINGIFY(l##x)
#define __SCNPTR(x) __STRINGIFY(l##x)

#define PRIdPTR		__PRIPTR(d)
#define PRIiPTR		__PRIPTR(i)
#define PRIoPTR		__PRIPTR(o)
#define PRIuPTR		__PRIPTR(u)
#define PRIxPTR		__PRIPTR(x)
#define PRIXPTR		__PRIPTR(X)

#define SCNdPTR		__SCNPTR(d)
#define SCNiPTR		__SCNPTR(i)
#define SCNoPTR		__SCNPTR(o)
#define SCNuPTR		__SCNPTR(u)
#define SCNxPTR		__SCNPTR(x)


#endif /* inttypes_h_ */
