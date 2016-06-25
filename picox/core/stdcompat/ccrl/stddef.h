/**
 *       @file  stddef.h
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

#ifndef stddef_h_
#define stddef_h_


typedef signed int ptrdiff_t;
typedef unsigned int size_t;

#ifndef NULL
#define NULL    ((void *)0)
#endif  /* !NULL */

#define offsetof(type,member)   ((size_t)&(((type *)0)->member))

#if defined(__CNV_NC30__) || defined(__CNV_IAR__)
typedef unsigned short wchar_t;
#endif

/* 本当はSEEK_XXXはstdio.hで定義されるべきだが、CA78K0Rのstdio.hがstddef.hを
 * includeしているので、stddef.hで定義しても辻褄が合う。
 */
#define SEEK_SET                        (0)
#define SEEK_CUR                        (1)
#define SEEK_END                        (2)

#endif /* stddef_h_ */
