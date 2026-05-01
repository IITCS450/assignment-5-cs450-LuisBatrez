#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

#define MAX_THREADS 8
#define STACK_SIZE 4096

enum state {
    FREE,
    RUNNABLE,
    RUNNING,
    DONE
};

struct thread {
    int id;
    enum state state;
    uint *sp;
    char stack[STACK_SIZE];
    void (*fn)(void*);
    void *arg;
};

static struct thread threads[MAX_THREADS];
static int current = 0;
static int initialized = 0;

extern void uswtch(uint **old, uint *new);

static void trampoline() {
    int id = current;

    threads[id].fn(threads[id].arg);

    threads[id].state = DONE;

    while (1)
        thread_yield();
}

void thread_init(void) {
    int i;

    if (initialized) return;

    for (i = 0; i < MAX_THREADS; i++) {
        threads[i].state = FREE;
    }

    threads[0].state = RUNNING;
    current = 0;
    initialized = 1;
}

tid_t thread_create(void (*fn)(void*), void *arg) {
    int i;
    uint *sp;

    if (!initialized)
        thread_init();

    for (i = 1; i < MAX_THREADS; i++) {
        if (threads[i].state == FREE) {
            threads[i].id = i;
            threads[i].state = RUNNABLE;
            threads[i].fn = fn;
            threads[i].arg = arg;

            sp = (uint*)(threads[i].stack + STACK_SIZE);

            // align stack
            sp = (uint*)((uint)sp & ~3);

            // fake stack frame
            *--sp = (uint)trampoline;
            *--sp = 0;
            *--sp = 0;
            *--sp = 0;
            *--sp = 0;

            threads[i].sp = sp;

            return i;
        }
    }

    return -1;
}

void thread_yield(void) {
    int old = current;
    int next = -1;
    int i;

    for (i = 1; i <= MAX_THREADS; i++) {
        int candidate = (old + i) % MAX_THREADS;

        if (threads[candidate].state == RUNNABLE) {
            next = candidate;
            break;
        }
    }

    if (next == -1)
        return;

    // 🔥 FIX: don’t overwrite DONE threads
    if (threads[old].state == RUNNING)
        threads[old].state = RUNNABLE;

    threads[next].state = RUNNING;
    current = next;

    uswtch(&threads[old].sp, threads[next].sp);
}

int thread_join(tid_t tid) {
    while (threads[tid].state != DONE) {
        thread_yield();
    }

    threads[tid].state = FREE;
    return 0;
}