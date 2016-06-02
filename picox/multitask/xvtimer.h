/**
 *       @file  xvtimer.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/14
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


#ifndef picox_multitask_xvtimer_h_
#define picox_multitask_xvtimer_h_


#include <picox/core/xcore.h>
#include <picox/container/xintrusive_list.h>


/** @addtogroup multitask
 *  @{
 *  @addtogroup xvtimer
 *  @brief  時間管理によるコールバック関数呼び出し
 *
 *  組込み開発ではタイマーを使った時間指定によるコールバック呼び出しは必須ですよね。
 *  このモジュールではコールバック関数のスケジューリングを支援します。
 *  排他処理は行っていないのでタイマのハードウェア割り込みを伴う用途に使用する場
 *  合は、このモジュールの関数を適当にラップしてやってください。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct XVTimer;
struct XVTimerRequest;
typedef struct XVTimer XVTimer;
typedef struct XVTimerRequest XVTimerRequest;


/** @brief コールバック関数ポインタ型です */
typedef void (*XVTimerCallBack)(void* arg);


/** @brief コールバックリクエスト構造体です
 *
 *  このオブジェクトはスタック以外の領域に確保する必要があります。
 */
struct XVTimerRequest
{
    /** @readonlysection */
    XVTimerCallBack     callback;
    void*               arg;
    XTicks              delay;
    XTicks              interval;
    XDeleter            deleter;

    /** @privatesection */
    XIntrusiveNode      m_node;
    XTicks              m_count;
    XVTimer*            m_holder;
    unsigned            m_has_marked_for_deletion : 1;
    unsigned            m_pending                 : 1;
    unsigned            m_is_delayed              : 1;
    unsigned            m_once                    : 1;
};


/** @brief 仮想タイマー構造体です
 */
struct XVTimer
{
/** @privatesection */
    XIntrusiveList      m_requests;
    XIntrusiveList      m_pending_requests;
    XTicks              m_tick_count;
    bool                m_in_scheduling;
};


/** @brief 仮想タイマオブジェクトを初期化します
 */
void xvtimer_init(XVTimer* self);


/** @brief コールバックリクエストオブジェクトを初期化します
 */
void xvtimer_init_request(XVTimerRequest* request);


/** @brief リクエストを全て除去します
 */
void xvtimer_deinit(XVTimer* self);


/** @brief コールバックリクエストを追加します
 *
 *  @param request  初期化済みのリクエストオブジェクト
 *  @param callback コールバック関数
 *  @param arg      コールバック関数の引数
 *  @param delay    intervalのカウントを開始するまでの遅延時間
 *  @param interval コールバックの呼び出し周期
 *  @param once     初回のコールバック呼び出しでリクエストを除去するかどうか
 *  @param deleter  リクエストを除去する時に呼び出すデリータ
 */
void xvtimer_add_request(XVTimer* self,
                         XVTimerRequest* request,
                         XVTimerCallBack callback,
                         void* arg,
                         XTicks delay,
                         XTicks interval,
                         bool once,
                         XDeleter deleter);


/** @brief リクエストを除去します
 *
 *  コールバック関数の呼び出し中に自分自身や他のリクエストを除去することも可能で
 *  す
 */
void xvtimer_remove_requst(XVTimer* self, XVTimerRequest* request);


/** @brief 仮想タイマのカウントをstep数ぶん進めます
 *
 *  タイマのカウントは自動的には行われないので、利用者がこの関数を呼び出して自分
 *  でカウントを進める必要があります。
 */
void xvtimer_schedule(XVTimer* self, XTicks step);


/** @brief 仮想タイマの現在時刻を返します
 */
XTicks xvtimer_now(const XVTimer* self);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xvtimer
 *  @} end of addtogroup multitask
 */


#endif /* picox_multitask_xvtimer_h_ */
