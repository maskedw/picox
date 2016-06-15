/**
 *       @file  xfiber_def.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/06
 * ===================================================================
 */


/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_multitask_xfiber_def_h_
#define picox_multitask_xfiber_def_h_


#include <picox/core/xcore.h>
#include <picox/container/xintrusive_list.h>
#include <setjmp.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef int32_t XTick;
typedef void* (*XFiberFunc)(void*);

#define X_TICK_TIMEOUT_FOREVER      ((XTick)-1)
#define X_FIBER_EVENT_WAIT_OR       (0)
#define X_FIBER_EVENT_WAIT_AND      (1)
#define X_FIBER_EVENT_WAIT_MASK     (0x01)
#define X_FIBER_EVENT_CLEAR_ON_EXIT (1 << 1)

struct XFiber;
struct XFiberEvent;
typedef struct XFiber XFiber;
typedef struct XFiberEvent XFiberEvent;


typedef enum
{
    X_FIBER_STATE_READY,
    X_FIBER_STATE_RUNNING,
    X_FIBER_STATE_DESTROYED,
    X_FIBER_STATE_WAITING_EVENT,
} XFiberState;

#define X_FIBER_STATE_IS_WAITING(state)     ((state) >= X_FIBER_STATE_WAITING_EVENT)


struct XFiber
{
/** @privatesection */
    XIntrusiveNode  m_node;
    size_t          m_stack_size;
    int             m_priority;
    XFiberFunc      m_func;
    void*           m_arg;
    void*           m_frame;
    XFiberState     m_state;
    XBits           m_wait_event_pattern;
    XMode           m_wait_event_mode;
    XBits*          m_result_event_patten;
    XError*         m_result_waiting;
    char            m_name[32];
};


struct XFiberEvent
{
    XIntrusiveList m_queue;
    XBits          m_pattern;
    char           m_name[32];
};


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_multitask_xfiber_def_h_ */
