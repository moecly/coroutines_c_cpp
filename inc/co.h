#ifndef __CO_H__
#define __CO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(CO_USE_SETJMP)
#include <setjmp.h>
#endif

/* coroutines status values */
#define CO_STATUS_BLOCKED 0
#define CO_STATUS_FINISHED -1
#define CO_STATUS_YIELDED -2

#define __co_line2(name, line) __co_##name_##line
#define __co_line(name) __co_line2(name, __LINE__)

#define co_new_mem malloc

typedef struct co {
    int status;
    uint8_t *heap;
    uint32_t heap_use;
    uint32_t heap_size;
#if defined(CO_USE_SETJMP)
    jmp_buf env;
    int is_set;
#else
    uint32_t label;
#endif
} co_t;

#if defined(CO_USE_SETJMP)

#define co_init(co, size)                                                   \
    ({                                                                      \
        (co)->is_set = 0;                                                   \
        (co)->status = 0;                                                   \
        (co)->heap = (uint8_t *)co_new_mem(size);                           \
        (co)->heap_use = 0;                                                 \
        (co)->heap_size = size;                                             \
        (void *)((co)->heap ? (co)->heap : NULL);                           \
    })

#define co_begin(co)                                                        \
    do {                                                                    \
        if ((co)->is_set) {                                                 \
            longjmp((co)->env, 0);                                          \
        }                                                                   \
    } while (0)

#define co_label(co, stat)                                                  \
    do {                                                                    \
        (co)->heap_use = 0;                                                 \
        (co)->is_set = 1;                                                   \
        (co)->status = (stat);                                              \
        setjmp((co)->env);                                                  \
    } while (0)

#define co_end(co) co_label(co, CO_STATUS_FINISHED)

#define co_deinit(co)                                                       \
    free((co)->heap)

#else

#define co_init(co, size)                                                   \
    ({                                                                      \
        (co)->label = 0;                                                    \
        (co)->status = 0;                                                   \
        (co)->heap = (uint8_t *)co_new_mem(size);                           \
        (co)->heap_use = 0;                                                 \
        (co)->heap_size = size;                                             \
        (void *)((co)->heap ? (co)->heap : NULL);                           \
    })

#define co_begin(co)                                                        \
    switch ((co)->label) {                                                  \
    case 0:

#define co_label(co, stat)                                                  \
    do {                                                                    \
        (co)->heap_use = 0;                                                 \
        (co)->label = __LINE__;                                             \
        (co)->status = stat;                                                \
    case __LINE__:;                                                         \
    } while (0)

#define co_end(co)                                                          \
    co_label(co, CO_STATUS_FINISHED);                                       \
    }

#define co_deinit(co)                                                       \
    free((co)->heap)

#endif

/*
 * Core coroutines API
 */
#define co_status(co) (co)->status

#define co_wait(co, cond)                                                   \
    do {                                                                    \
        co_label(co, CO_STATUS_BLOCKED);                                    \
        if (!(cond)) return;                                                \
    } while (0)


#define co_yield(co)                                                        \
    do {                                                                    \
        co_label(co, CO_STATUS_YIELDED);                                    \
        if (co_status(co) == CO_STATUS_YIELDED) {                           \
            co_status(co) = CO_STATUS_BLOCKED;                              \
            return;                                                         \
        }                                                                   \
    } while (0)

#define co_exit(co, stat)                                                   \
    do {                                                                    \
        co_label(co, stat);                                                 \
        return;                                                             \
    } while (0)

#define co_new(co, size)                                                    \
    (void *)((((co)->heap_use) + (size)) <= (co)->heap_size ?               \
    (&(co)->heap[(co)->heap_use]) : NULL);                                  \
    (co)->heap_use += size;

#ifdef __cplusplus
}
#endif

#endif // __CO_H__
