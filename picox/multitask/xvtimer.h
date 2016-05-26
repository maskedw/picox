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


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct XVTimer;
struct XVTimerRequest;
typedef struct XVTimer XVTimer;
typedef struct XVTimerRequest XVTimerRequest;


typedef void (*XVTimerCallBack)(void* arg);


struct XVTimerRequest
{
    XVTimerCallBack     callback;
    void*               arg;
    XTicks              delay;
    XTicks              interval;

    /** @privatesection */
    XIntrusiveNode      m_node;
    XTicks              m_count;
    XVTimer*            m_holder;
    unsigned            m_has_marked_for_deletion : 1;
    unsigned            m_pending                 : 1;
    unsigned            m_is_delayed              : 1;
    unsigned            m_once                    : 1;
};


struct XVTimer
{
/** @privatesection */
    XIntrusiveList      m_requests;
    XIntrusiveList      m_pending_requests;
    XTicks              m_tick_count;
    bool                m_in_scheduling;
};


void xvtimer_init(XVTimer* self);
void xvtimer_init_request(XVTimerRequest* request);
void xvtimer_deinit(XVTimer* self, XDeleter deleter);
void xvtimer_add_request(XVTimer* self,
                         XVTimerRequest* request,
                         XVTimerCallBack callback,
                         void* arg,
                         XTicks delay,
                         XTicks interval,
                         bool once);
void xvtimer_remove_requst(XVTimer* self, XVTimerRequest* request);
void xvtimer_schedule(XVTimer* self, XTicks step);
XTicks xvtimer_now(const XVTimer* self);
XTicks xvtimer_elapsed(const XVTimer* self, XTicks start);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_multitask_xvtimer_h_ */
