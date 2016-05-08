/**
 *       @file  xfiber.h
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


#ifndef picox_multitask_xfiber_h_
#define picox_multitask_xfiber_h_


#include <picox/multitask/xfiber_def.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


XError xfiber_kernel_init(size_t total_stack_size, size_t main_stack_size);
void xfiber_yield();
XFiber* xfiber_self();
XError xfiber_init(XFiber* fiber, const char* name, size_t stack_size, XFiberFunc func, void* arg, int priority);
XError xfiber_deinit(XFiber* fiber);
const char* xfiber_name(const XFiber* fiber);

XError xfiber_event_init(XFiberEvent* event, const char* name);
XError xfiber_event_deinit(XFiberEvent* event);
XError xfiber_event_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);
XError xfiber_event_try_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);
XError xfiber_event_timed_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result, XTick timeout);
XError xfiber_event_set(XFiberEvent* event, XBits pattern);
XError xfiber_event_set_isr(XFiberEvent* event, XBits pattern);
XError xfiber_event_clear(XFiberEvent* event, XBits pattern);
const char* xfiber_event_name(const XFiberEvent* event);



// XFiberMailbox
// XFiberSemaphore

// XError xfiber_semaphore_init();
// XError xfiber_channel_init();
// /* シングルリスト */
// XError xfiber_mailbox_init();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_multitask_xfiber_h_ */
