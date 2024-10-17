#ifndef __CO_H__
#define __CO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Protothread status values */
#define CO_STATUS_BLOCKED 0
#define CO_STATUS_FINISHED -1
#define CO_STATUS_YIELDED -2

#define __co_line2(name, line) __co_##name_##line
#define __co_line(name) __co_line2(name, __LINE__)

#if defined(CO_USE_SETJMP)
#include <setjmp.h>

typedef struct co {
    jmp_buf env;
    int is_set;
    int status;
} co_t;

#define co_init(co)                                                         \
    do {                                                                    \
        (co)->is_set = 0;                                                   \
        (co)->status = 0;                                                   \
    } while (0)

#define co_begin(co)                                                        \
    do {                                                                    \
        if ((co)->is_set) {                                                 \
            longjmp((co)->env, 0);                                          \
        }                                                                   \
    } while (0)

#define co_label(co, stat)                                                  \
    do {                                                                    \
        (co)->is_set = 1;                                                   \
        (co)->status = (stat);                                              \
        setjmp((co)->env);                                                  \
    } while (0)

#define co_end(co) co_label(co, CO_STATUS_FINISHED)

#else

typedef struct co {
    int label;
    int status;
} co_t;

#define co_init(co)                                                         \
    do {                                                                    \
        (co)->label = 0;                                                    \
        (co)->status = 0;                                                   \
    } while (0)

#define co_begin(co)                                                        \
    switch ((co)->label) {                                                  \
    case 0:

#define co_label(co, stat)                                                  \
    do {                                                                    \
        (co)->label = __LINE__;                                             \
        (co)->status = stat;                                                \
    case __LINE__:;                                                         \
    } while (0)

#define co_end(co)                                                          \
    co_label(co, CO_STATUS_FINISHED);                                       \
    }

#endif

/*
 * Core co API
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

#ifdef __cplusplus
}
#endif

#endif // __CO_H__
