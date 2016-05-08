/**
 *       @file  xfiber.c
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
 * modify, merge, publish, X__GetStackDepthribute, sublicense, and/or sell copies
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


#include <picox/multitask/xfiber.h>


#define MIN_STACKSIZE           400     // minimum stack size
#define PRINTF(...) do { printf(__VA_ARGS__); fflush(stdout); } while (0)


struct X__StackFrame;
typedef struct X__StackFrame X__StackFrame;

struct X__StackFrame
{
    X__StackFrame*  m_next;
    XFiber*         m_fiber;
    size_t          m_block_size;
    bool            m_used;
    jmp_buf         m_jmpbuf;
};


typedef struct X__Kernel
{
    XFiber*         m_cur_task;
    X__StackFrame   m_main_frame;
    XFiber          m_main_task;
    XIntrusiveList  m_ready_queue;
    jmp_buf         m_tmp_jmpbuf;
} X__Kernel;


static size_t X__GetStackDepth(const X__StackFrame* from, const X__StackFrame* to);
static void X__EatStack(X__StackFrame* p, size_t block_size);
static void X__PushReadyQueue(XFiber* fiber);
static XFiber* X__PopReadyQueue(void);
static void X__Schedule(void);
static bool X__TestEvent(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);


X__Kernel        x_g_fiber_kernel;
#define priv    (&x_g_fiber_kernel)


XError xfiber_kernel_init(size_t total_stack_size, size_t main_stack_size)
{
    xilist_init(&priv->m_ready_queue);

    /* init main fiber */
    priv->m_main_task.m_func = NULL;
    priv->m_main_task.m_arg = NULL;
    priv->m_main_task.m_priority = 0;
    priv->m_main_task.m_stack_size = 0;
    priv->m_main_task.m_frame = &priv->m_main_frame;
    strcpy(priv->m_main_task.m_name, "main");

    /* Make main stack farme */
    X__StackFrame tmp;
    tmp.m_next = NULL;
    tmp.m_block_size = total_stack_size;
    tmp.m_used = false;
    tmp.m_fiber = &priv->m_main_task;
    if (setjmp(tmp.m_jmpbuf) == 0)
        X__EatStack(&tmp, main_stack_size);
    tmp.m_used = true;
    priv->m_main_frame = tmp;

    priv->m_main_task.m_frame = &priv->m_main_frame;
    priv->m_main_task.m_state = X_FIBER_STATE_RUNNING;
    priv->m_cur_task = &priv->m_main_task;
}


void xfiber_yield()
{
    X__StackFrame* const cur_frame = priv->m_cur_task->m_frame;
    if (setjmp(cur_frame->m_jmpbuf) == 0)
        X__Schedule();
}


XFiber* xfiber_self(void)
{
    return priv->m_cur_task;
}


XError xfiber_init(XFiber* fiber, const char* name, size_t stack_size, XFiberFunc func, void* arg, int priority)
{
    X__StackFrame* p;

    for (p = priv->m_main_frame.m_next; p != NULL; p = p->m_next)
    {
        if (!p->m_used && p->m_block_size >= stack_size)
        {
            fiber->m_func = func;
            fiber->m_arg = arg;
            fiber->m_priority = priority;
            fiber->m_stack_size = stack_size;
            if (name)
                strcpy(fiber->m_name, name);
            p->m_fiber = fiber;

            if (setjmp (priv->m_tmp_jmpbuf) == 0)
                longjmp (p->m_jmpbuf, 1);

            X__PushReadyQueue(fiber);
            return X_ERR_NONE;
        }
    }

    return X_ERR_OTHER;
}


XError xfiber_deinit(XFiber* fiber)
{
    /* TODO 終了処理 */
    X_UNUSED(fiber);
    return X_ERR_NONE;
}


const char* xfiber_name(const XFiber* fiber)
{
    return fiber->m_name;
}


XError xfiber_event_init(XFiberEvent* event, const char* name)
{
    strcpy(event->m_name, name);
    event->m_pattern = 0;
    xilist_init(&event->m_queue);

    return X_ERR_NONE;
}


XError xfiber_event_deinit(XFiberEvent* event)
{
    X_UNUSED(event);
    return X_ERR_NONE;
}


XError xfiber_event_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result)
{
    XError err = X_ERR_NONE;

    if (X__TestEvent(event, mode, wait_pattern, result))
        goto x__exit;

    XFiber* const fiber = xfiber_self();
    fiber->m_result_event_patten = result;
    fiber->m_wait_event_pattern = wait_pattern;
    fiber->m_wait_event_mode = mode;
    fiber->m_state = X_FIBER_STATE_WAITING_EVENT;
    fiber->m_result_waiting = &err;
    xilist_push_front(&event->m_queue, &fiber->m_node);

    X__Schedule();
    err = *(fiber->m_result_waiting);

x__exit:
    return err;
}


XError xfiber_event_set(XFiberEvent* event, XBits pattern)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    event->m_pattern |= pattern;
    XIntrusiveNode* ite = xilist_front(&event->m_queue);
    XIntrusiveNode* const end = xilist_end(&event->m_queue);

    while (ite != end)
    {
        XFiber* const fiber = xnode_entry(ite, XFiber, m_node);
        XIntrusiveNode* const next = ite->next;

        if (X__TestEvent(event,
                         fiber->m_wait_event_mode,
                         fiber->m_wait_event_pattern,
                         fiber->m_result_event_patten))
        {
            *(fiber->m_result_waiting) = X_ERR_NONE;
            xnode_unlink(&fiber->m_node);
            X__PushReadyQueue(fiber);
            scheduling_request = true;
            if (fiber->m_wait_event_mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
                break;
        }
        ite = next;
    }

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_event_clear(XFiberEvent* event, XBits pattern)
{
    event->m_pattern &= ~pattern;
    return X_ERR_NONE;
}


static size_t X__GetStackDepth(const X__StackFrame* from, const X__StackFrame* to)
{
    const uint8_t* c1, *c2;
    c1 = (const uint8_t*)from;
    c2 = (const uint8_t*)to;

    if (c1 > c2)                          // stack grows down
        return c1 - c2;
    else                                  // stack grows up
        return c2 - c1;
}


static void X__EatStack(X__StackFrame* p, size_t block_size)
{
    X__StackFrame t;
    const size_t depth = X__GetStackDepth(p, &t);

    if (depth < block_size)
        X__EatStack(p, block_size);

    t.m_block_size = p->m_block_size - depth;
    p->m_block_size = depth;
    t.m_used = false;
    t.m_next = p->m_next;
    p->m_next = &t;

    if (setjmp (t.m_jmpbuf) == 0)
        longjmp (p->m_jmpbuf, 1);

    PRINTF("enter for loop %p\n", &t);
    for (;;)
    {
        if (t.m_fiber->m_stack_size + MIN_STACKSIZE <= t.m_block_size)         // test m_block_size
        {
            PRINTF("split block %p\n", &t);
            if (setjmp (t.m_jmpbuf) == 0)         // split block
                X__EatStack (&t, t.m_fiber->m_stack_size);
        }

        t.m_used = true;                      // mark as m_used
        t.m_fiber->m_frame = &t;
        PRINTF("mark as m_used after wait %p\n", &t);
        if (setjmp (t.m_jmpbuf) == 0)           // wait
            longjmp (priv->m_tmp_jmpbuf, 1);

        PRINTF("run task %p\n", &t);

        t.m_fiber->m_func(t.m_fiber->m_arg);
        t.m_fiber->m_state = X_FIBER_STATE_DESTROYED;
        t.m_used = false;                     // mark as free

        PRINTF("func end %p\n", &t);

        if (t.m_next != NULL && !t.m_next->m_used)
        {
            t.m_block_size += t.m_next->m_block_size;           // merge with following block
            t.m_next = t.m_next->m_next;
        }

        p = priv->m_main_frame.m_next;                 // loop through list
        if (p != &t)                        // if not first block
        {
            while (p->m_next != &t)             // locate previous block
            {
                p = p->m_next;
            }

            if (!p->m_used)                     // if free
            {
                p->m_block_size += t.m_block_size;              // then merge
                p->m_next = t.m_next;
            }
        }

        PRINTF("save state %p\n", &t);
        if (setjmp (t.m_jmpbuf) == 0)           // save state
            X__Schedule();
        PRINTF("restore state %p\n", &t);
        volatile int nop = 0;
        nop++;
    }
    PRINTF("reutrn X__EatStack %p\n", &t);
}


static void X__PushReadyQueue(XFiber* fiber)
{
    /* priority */
    fiber->m_state = X_FIBER_STATE_READY;
    xilist_push_back(&priv->m_ready_queue, &fiber->m_node);
}


/* TODO priority */
static XFiber* X__PopReadyQueue(void)
{
    XIntrusiveNode* const next = xilist_front(&priv->m_ready_queue);
    if (next == xilist_end(&priv->m_ready_queue))
    {
        puts("Deadlock!");
        exit(1);
    }

    xnode_unlink(next);
    XFiber* const fiber = xnode_entry(next, XFiber, m_node);
    return fiber;
}


static void X__Schedule(void)
{
    XFiber* fiber = priv->m_cur_task;
    if (fiber->m_state == X_FIBER_STATE_RUNNING)
        X__PushReadyQueue(fiber);

    fiber = X__PopReadyQueue();
    fiber->m_state = X_FIBER_STATE_RUNNING;
    priv->m_cur_task = fiber;

    X__StackFrame* const cur_frame = fiber->m_frame;
    longjmp(cur_frame->m_jmpbuf, 1);
}


static bool X__TestEvent(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result)
{
    bool ok = false;
    const XBits pattern = event->m_pattern;

    if ((mode & X_FIBER_EVENT_WAIT_MASK) == X_FIBER_EVENT_WAIT_OR)
        ok = (pattern & wait_pattern) != 0;
    else
        ok = (pattern & wait_pattern) == wait_pattern;

    if (ok)
    {
        *result = pattern;
        if (mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
            event->m_pattern = 0;
    }

    return ok;
}
