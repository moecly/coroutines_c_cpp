#include <stdio.h>

#include "co.h"

void test0(co_t *co) {
    static int i = 0;
    co_begin(co);
    while (i < 10) {
        printf("test0 = %d\n", i);
        i++;
        co_yield(co);
    }
    co_end(co);
}

void test1(co_t *co) {
    static int i = 0;
    co_begin(co);
    while (i < 10) {
        printf("test1 = %d\n", i);
        i++;
        co_yield(co);
    }
    co_end(co);
}

int main(void) {
    co_t co0;
    co_t co1;
    co_init(&co0);
    co_init(&co1);
    while (1) {
        test0(&co0);
        test1(&co1);
    }
    return 0;
}