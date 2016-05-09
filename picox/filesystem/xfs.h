/**
 *       @file  xfs.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/06
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
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

#ifndef picox_filesystem_xfs_h_
#define picox_filesystem_xfs_h_


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xfs
 *  @brief picox標準のファイルシステムインターフェースです
 *
 *  X_CONF_XFS_TYPEの設定によって、xsinglefs or xunionfsの関数名に置換されます。
 *  初期化関数はタイプによってことなるので、設定値によって適切な初期をおこなって
 *  ください。
 *
 *  @see X_CONF_XFS_TYPE
 *  @{
 */


#if X_XFS_TYPE == X_XFS_TYPE_UNION_FS

    #include <picox/filesystem/xunionfs.h>

    #define xfs_init_stream     xunionfs_init_stream
    #define xfs_deinit          xunionfs_deinit
    #define xfs_open            xunionfs_open
    #define xfs_close           xunionfs_close
    #define xfs_read            xunionfs_read
    #define xfs_write           xunionfs_write
    #define xfs_seek            xunionfs_seek
    #define xfs_tell            xunionfs_tell
    #define xfs_flush           xunionfs_flush
    #define xfs_mkdir           xunionfs_mkdir
    #define xfs_opendir         xunionfs_opendir
    #define xfs_readdir         xunionfs_readdir
    #define xfs_closedir        xunionfs_closedir
    #define xfs_chdir           xunionfs_chdir
    #define xfs_getcwd          xunionfs_getcwd
    #define xfs_remove          xunionfs_remove
    #define xfs_rename          xunionfs_rename
    #define xfs_stat            xunionfs_stat
    #define xfs_utime           xunionfs_utime
    #define xfs_putc            xunionfs_putc
    #define xfs_puts            xunionfs_puts
    #define xfs_printf          xunionfs_printf
    #define xfs_vprintf         xunionfs_vprintf
    #define xfs_getc            xunionfs_getc
    #define xfs_gets        xunionfs_gets
    #define xfs_copyfile        xunionfs_copyfile
    #define xfs_copyfile2       xunionfs_copyfile2
    #define xfs_copytree        xunionfs_copytree
    #define xfs_rmtree          xunionfs_rmtree
    #define xfs_makedirs        xunionfs_makedirs
    #define xfs_walktree        xunionfs_walktree
    #define xfs_exists          xunionfs_exists
    #define xfs_is_directory    xunionfs_is_directory
    #define xfs_is_regular      xunionfs_is_regular

#elif X_XFS_TYPE == X_XFS_TYPE_SINGLE_FS

    #include <picox/filesystem/xsinglefs.h>

    #define xfs_init_stream     xsinglefs_init_stream
    #define xfs_deinit          xsinglefs_deinit
    #define xfs_open            xsinglefs_open
    #define xfs_close           xsinglefs_close
    #define xfs_read            xsinglefs_read
    #define xfs_write           xsinglefs_write
    #define xfs_seek            xsinglefs_seek
    #define xfs_tell            xsinglefs_tell
    #define xfs_flush           xsinglefs_flush
    #define xfs_mkdir           xsinglefs_mkdir
    #define xfs_opendir         xsinglefs_opendir
    #define xfs_readdir         xsinglefs_readdir
    #define xfs_closedir        xsinglefs_closedir
    #define xfs_chdir           xsinglefs_chdir
    #define xfs_getcwd          xsinglefs_getcwd
    #define xfs_remove          xsinglefs_remove
    #define xfs_rename          xsinglefs_rename
    #define xfs_stat            xsinglefs_stat
    #define xfs_utime           xsinglefs_utime
    #define xfs_putc            xsinglefs_putc
    #define xfs_puts            xsinglefs_puts
    #define xfs_printf          xsinglefs_printf
    #define xfs_vprintf         xsinglefs_vprintf
    #define xfs_getc            xsinglefs_getc
    #define xfs_gets        xsinglefs_gets
    #define xfs_copyfile        xsinglefs_copyfile
    #define xfs_copyfile2       xsinglefs_copyfile2
    #define xfs_copytree        xsinglefs_copytree
    #define xfs_rmtree          xsinglefs_rmtree
    #define xfs_makedirs        xsinglefs_makedirs
    #define xfs_walktree        xsinglefs_walktree
    #define xfs_exists          xsinglefs_exists
    #define xfs_is_directory    xsinglefs_is_directory
    #define xfs_is_regular      xsinglefs_is_regular

#else

    #error Invalid XFS_TYPE

#endif


/** @} end of addtogroup xfs
 *  @} end of addtogroup filesystem
 */


#endif /* picox_filesystem_xfs_h_ */
