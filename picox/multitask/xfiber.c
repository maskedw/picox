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


X__Kernel        x_g_fiber_kernel;
#define priv    (&x_g_fiber_kernel)


static void schedule()                  // run m_next task
{
    XIntrusiveNode* cur = xilist_front(&priv->m_ready_queue);
    if (cur == xilist_end(&priv->m_ready_queue))
    {
        puts ("Deadlock!");
        exit (1);
    }

    xnode_unlink(cur);
    priv->m_cur_task = xnode_entry(cur, XFiber, m_node);
    X__StackFrame* const cur_frame = priv->m_cur_task->m_frame;
    longjmp (cur_frame->m_jmpbuf, 1);          // restore state of m_next task
}


static unsigned dist (X__StackFrame* from, X__StackFrame* to)
{
    char* c1, *c2;
    c1 = (char*) from;
    c2 = (char*) to;

    if (c1 > c2)                          // stack grows down
        return ( (unsigned) (c1 - c2));
    else                                  // stack grows up
        return ( (unsigned) (c2 - c1));
}


static void eat(X__StackFrame* p, unsigned block_size)
{
    X__StackFrame t;
    const size_t depth = dist (p, &t);

    if (depth < block_size)                         // eat stack
        eat(p, block_size);

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
                eat (&t, t.m_fiber->m_stack_size);
        }

        t.m_used = true;                      // mark as m_used
        t.m_fiber->m_frame = &t;
        PRINTF("mark as m_used after wait %p\n", &t);
        if (setjmp (t.m_jmpbuf) == 0)           // wait
            longjmp (priv->m_tmp_jmpbuf, 1);

        PRINTF("run task %p\n", &t);

        t.m_fiber->m_func(t.m_fiber->m_arg);
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
            schedule();
        PRINTF("restore state %p\n", &t);
        volatile int nop = 0;
        nop++;
    }
    PRINTF("reutrn eat %p\n", &t);
}


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
        eat(&tmp, main_stack_size);
    tmp.m_used = true;
    priv->m_main_frame = tmp;

    priv->m_main_task.m_frame = &priv->m_main_frame;
    priv->m_cur_task = &priv->m_main_task;
}


XFiber* xfiber_self(void)
{
    return priv->m_cur_task;
}


const char* xfiber_name(const XFiber* fiber)
{
    return fiber->m_name;
}


void xfiber_yield()
{
    X__StackFrame* const frame = priv->m_cur_task->m_frame;
    if (setjmp(frame->m_jmpbuf) == 0)
    {
        xilist_push_back(&priv->m_ready_queue, &priv->m_cur_task->m_node);
        schedule();
    }
}


XError xfiber_init(XFiber* fiber, const char* name, size_t stack_size, XFiberFunc func, void* arg, int priority)
{
    X__StackFrame* p;

    for (p = priv->m_main_frame.m_next; p != NULL; p = p->m_next)      // find free block
    {
        if (!p->m_used && p->m_block_size >= stack_size)
        {
            fiber->m_func = func;                     // set task parameters
            fiber->m_arg = arg;
            fiber->m_priority = priority;
            fiber->m_stack_size = stack_size;
            if (name)
                strcpy(fiber->m_name, name);
            p->m_fiber = fiber;

            if (setjmp (priv->m_tmp_jmpbuf) == 0)       // activate control block
                longjmp (p->m_jmpbuf, 1);

            xilist_push_back(&priv->m_ready_queue, &fiber->m_node);
            return X_ERR_NONE;
        }
    }

    return X_ERR_OTHER;
}
