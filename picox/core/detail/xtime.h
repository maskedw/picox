/**
 *       @file  xtime.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/03/21
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


#ifndef picox_core_detail_xtime_h_
#define picox_core_detail_xtime_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xtime
 *  @brief 時間を扱うための関数等
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @see X_CONF_TICKS_PER_SEC */
#define X_TICKS_PER_SEC  X_CONF_TICKS_PER_SEC


/** @brief システムチックを格納する型です
 */
typedef int32_t XTicks;


/** @brief 待ち関数で永久待ちを表す特殊値です
 *
 *  マルチタスク系の特定の関数でのみ有効です。
 */
#define X_TICKS_FOREVER      ((XTicks)-1)


/** @brief time_tの代替をするシステム時刻を格納するための型です
 *
 *  POSIX互換システム風にUNIX時間1970年1月1日0時0分0秒(ただしタイムゾーンは考慮
 *  しない)からの経過秒数を表します。
 *  旧いシステムではtime_tは32bit符号ありで実装されることが多く、2038年問題があ
 *  りますが、XTimeは符号なしなので2106年まで保持可能です。
 */
typedef uint32_t XTime;


/** @brief  高精度のシステム時刻を格納するための型です
 */
typedef struct HogeHoge
{
    XTime   tv_sec;     /** 秒 */
    int32_t tv_usec;    /** マイクロ秒 */
} XTimeVal;


/** @brief 秒単位の時間を格納する型です
 *
 *  XTimeはUNIX時間が基準ですが、XSecondsは単純な秒数として扱うことを意図してい
 *  ます。
 */
typedef int32_t XSeconds;


/** @brief ミリ秒単位の時間を格納する型です
 */
typedef int32_t XMSeconds;


/** @brief マイクロ秒単位の時間を格納する型です
 */
typedef int32_t XUSeconds;


/** @name user_porting_functions
 *  @{
 */

/** @brief 現在のチック時間を返します
 */
XTicks x_port_ticks_now(void);


/** @brief 現在時刻を返します
 *
 *  POSIX標準のgettimeofday()のpicox版です。本来は第２引数にタイムゾーンを指定し
 *  ますが、picoxは対応していないため、ダミーの引数として常に無視します。
 *
 *  ポインタを渡すのが面倒な時はx_gettimeofday2()を使用するとXTimeValを戻り値か
 *  ら直接取得できます。
 *
 *  https://linuxjm.osdn.jp/html/LDP_man-pages/man2/gettimeofday.2.html
 */
int x_port_gettimeofday(XTimeVal* tv, void* tz_dammy);


/** @brief ミリ秒単位のスリープを行います
 *
 *  精度は実装次第ですが、通常はチック周期に切り上げられると考えてください。
 *
 *  @note
 *  ユーザーが移植を行う場合、少なくとも指定時間以上の待ちを保証できるように実装
 *  してください。
 */
void x_port_msleep(XMSeconds msec);


/** @brief マイクロ秒単位のスリープを行います
 *
 *  @note
 *  注意点はx_port_msleep()と同様です。
 */
void x_port_usleep(XUSeconds usec);


/** @brief ミリ秒単位のディレイを行います
 *
 *  @note
 *  スリープとディレイの違いは、X_CONF_UDELAY_IMPL_TYPEのドキュメントを参照して
 *  ください。
 *
 *  @see X_CONF_MDELAY_IMPL_TYPE
 */
void x_port_mdelay(XMSeconds msec);


/** @brief マイクロ秒単位のディレイを行います
 *
 *  @note
 *  スリープとディレイの違いは、X_CONF_MDELAY_IMPL_TYPEのドキュメントを参照して
 *  ください。
 *
 *  @see X_CONF_UDELAY_IMPL_TYPE
 */
void x_port_udelay(XUSeconds usec);


#define x_ticks_now         x_port_ticks_now
#define x_gettimeofday      x_port_gettimeofday
#define x_msleep            x_port_msleep
#define x_usleep            x_port_usleep
#define x_mdelay            x_port_mdelay
#define x_udelay            x_port_mdelay


/** @} end of name user_porting_functions
 */


/** @name time_conversions
 *  @details
 *
 *  X秒からチックへの変換の際はチック単位に切り上げし、チックからX秒への変換は切
 *  り下げます。
 *  時間待ちが最終的にはチック単位で行われるため、指定時間以上の待ち時間を確保す
 *  ることがこの変換規則の目的です。
 *  @{
 */

/** @brief ミリ秒をチックに変換した値を返します
 */
XTicks x_msec_to_ticks(XMSeconds msec);


/** @brief マイクロ秒をチックに変換した値を返します
 */
XTicks x_usec_to_ticks(XUSeconds usec);


/** @brief チックをミリ秒に変換した値を返します
 */
XMSeconds x_ticks_to_msec(XTicks ticks);


/** @brief チックをマイクロ秒に変換した値を返します
 */
XUSeconds x_ticks_to_usec(XTicks ticks);


/** @} end of name time_convertions
 */


/** @name  time_conversion_macros
 *  @brief 同等の関数のマクロ版です
 *  @{
 */

#define X_MSEC_TO_TICKS(msec)   (X_DIV_ROUNDUP((msec) * X_TICKS_PER_SEC, 1000))
#define X_USEC_TO_TICKS(usec)   (X_DIV_ROUNDUP((usec) * X_TICKS_PER_SEC, 1000) / 1000)
#define X_TICKS_TO_MSEC(ticks)  (((ticks) * 1000) / X_TICKS_PER_SEC)
#define X_TICKS_TO_USEC(ticks)  ((((ticks) * 1000) / X_TICKS_PER_SEC) * 1000)

/** @} end of name time_convertion_macros
 */


/** @name timeval_operations
 *
 *  以下リンク先のstruct timevalに対する操作の互換マクロです。XTimeValに対して使
 *  用します。
 *
 *  https://linuxjm.osdn.jp/html/LDP_man-pages/man3/timeradd.3.html
 *  @{
 */
#define x_timerclear(tvp)                         ((tvp)->tv_sec = (tvp)->tv_usec = 0)

#define x_timerisset(tvp)                         ((tvp)->tv_sec || (tvp)->tv_usec)

#define x_timercmp(tvp, uvp, cmp)                   \
    (((tvp)->tv_sec == (uvp)->tv_sec) ?             \
        ((tvp)->tv_usec cmp (uvp)->tv_usec) :       \
        ((tvp)->tv_sec cmp (uvp)->tv_sec))

#define x_timeradd(tvp, uvp, vvp)                           \
    do {                                                    \
        (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;      \
        (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;   \
        if ((vvp)->tv_usec >= X_INT32_C(1000000))           \
        {                                                   \
            (vvp)->tv_sec++;                                \
            (vvp)->tv_usec -= X_INT32_C(1000000);           \
        }                                                   \
    } while (0)

#define x_timersub(tvp, uvp, vvp)                           \
    do {                                                    \
        (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;      \
        (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;   \
        if ((vvp)->tv_usec < 0)                             \
        {                                                   \
            (vvp)->tv_sec--;                                \
            (vvp)->tv_usec += X_INT32_C(1000000);           \
        }                                                   \
    } while (0)


/** @} end of name timeval_operations
 */


/** @brief 現在時刻を返します
 */
XTimeVal x_gettimeofday2(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xtime
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xtime_h_ */
