#include <stdio.h>

#include "co.h"

void test0(co_t *co) {
    int *i = (int *)co_new(co, sizeof(int));
    co_begin(co);
    *i = 0;
    while (*i < 10) {
        printf("test0 = %d\n", *i);
        (*i)++;
        co_yield(co);
    }
    co_end(co);
}

void test1(co_t *co) {
    int *i = (int *)co_new(co, sizeof(int));
    co_begin(co);
    *i = 0;
    while (*i < 10) {
        printf("test1 = %d\n", *i);
        (*i)++;
        co_yield(co);
    }
    co_end(co);
}

int main(void) {
    co_t co0;
    co_t co1;
    co_init(&co0, 1024);
    co_init(&co1, 1024);
    while (co_status(&co0) != CO_STATUS_FINISHED && 
           co_status(&co1) != CO_STATUS_FINISHED) {
        test0(&co0);
        test1(&co1);
    }
    co_deinit(&co0);
    co_deinit(&co1);
    return 0;
}