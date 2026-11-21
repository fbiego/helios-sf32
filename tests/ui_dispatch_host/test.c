#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "rtthread.h"
#include "helios_ui_dispatch.h"

static pthread_mutex_t irq = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local bool locked;
static pthread_t ui_thread;
static unsigned calls, mask, fifo;
static atomic_uint producers_done;

rt_base_t rt_hw_interrupt_disable(void)
{
    assert(!locked);
    assert(!pthread_mutex_lock(&irq));
    locked = true;
    return 0;
}
void rt_hw_interrupt_enable(rt_base_t level)
{
    (void)level;
    assert(locked);
    locked = false;
    assert(!pthread_mutex_unlock(&irq));
}
static void called(void *data)
{
    assert(!data && !locked && pthread_equal(pthread_self(), ui_thread));
    ++calls;
}
static void callback_a(void *data) { called(data); }
static void callback_b(void *data) { called(data); }
static void repeating(void *data)
{
    called(data);
    assert(helios_ui_dispatch_post(repeating));
}
#define CALLBACK(n) static void work_##n(void *data) \
    { called(data); mask |= 1u << n; assert(fifo++ == n); }
CALLBACK(0) CALLBACK(1) CALLBACK(2) CALLBACK(3)
CALLBACK(4) CALLBACK(5) CALLBACK(6) CALLBACK(7)
CALLBACK(8) CALLBACK(9) CALLBACK(10) CALLBACK(11)
CALLBACK(12) CALLBACK(13) CALLBACK(14) CALLBACK(15)
static void *producer(void *unused)
{
    (void)unused;
    for (unsigned i = 0; i < 100000; ++i) {
        assert(helios_ui_dispatch_post(callback_a));
        assert(helios_ui_dispatch_post(callback_b));
    }
    atomic_fetch_add(&producers_done, 1);
    return NULL;
}
int main(void)
{
    ui_thread = pthread_self();
    helios_ui_dispatch_poll();
    assert(!helios_ui_dispatch_post(NULL));
    for (unsigned i = 0; i < 100000; ++i)
        assert(helios_ui_dispatch_post(callback_a));
    assert(calls == 0);
    helios_ui_dispatch_poll(); assert(calls == 1);
    helios_ui_dispatch_poll(); assert(calls == 1);

    const helios_ui_work_t callbacks[] = {
        work_0, work_1, work_2, work_3, work_4, work_5, work_6, work_7,
        work_8, work_9, work_10, work_11, work_12, work_13, work_14, work_15,
    };
    for (unsigned i = 0; i < 16; ++i) assert(helios_ui_dispatch_post(callbacks[i]));
    assert(!helios_ui_dispatch_post(callback_b));
    assert(helios_ui_dispatch_post(work_0)); /* Coalesce even at capacity. */
    helios_ui_dispatch_poll(); assert(mask == 0xffff && fifo == 16);

    pthread_t threads[2];
    for (unsigned i = 0; i < 2; ++i) assert(!pthread_create(&threads[i], NULL, producer, NULL));
    while (atomic_load(&producers_done) != 2) helios_ui_dispatch_poll();
    for (unsigned i = 0; i < 2; ++i) assert(!pthread_join(threads[i], NULL));
    helios_ui_dispatch_poll();
    unsigned after = calls;
    helios_ui_dispatch_poll(); assert(calls == after);
    assert(helios_ui_dispatch_post(callback_b));
    helios_ui_dispatch_poll(); assert(calls == after + 1);

    assert(helios_ui_dispatch_post(repeating));
    helios_ui_dispatch_poll(); assert(calls == after + 2);
    helios_ui_dispatch_poll(); assert(calls == after + 3);
    puts("UI dispatch: FIFO, capacity, coalescing, 400k concurrent posts, UI-only execution and bounded self-rescheduling passed");
    return 0;
}
