#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "gc.h"     // for str_decref on cleanup

void stack_init(Stack *st) {
    st->cap = 16;
    st->len = 0;
    st->data = malloc(st->cap * sizeof(Value));
    if (!st->data) { perror("malloc"); exit(1); }
}
void stack_free(Stack *st) {
    for (size_t i = 0; i < st->len; ++i) {
        if (st->data[i].type == VAL_STR) str_decref(st->data[i].v.s);
    }
    free(st->data);
}
void stack_push(Stack *st, Value val) {
    if (st->len == st->cap) {
        st->cap *= 2;
        st->data = realloc(st->data, st->cap * sizeof(Value));
        if (!st->data) { perror("realloc"); exit(1); }
    }
    st->data[st->len++] = val;
}
Value stack_pop(Stack *st) {
    if (st->len == 0) { fprintf(stderr, "Stack underflow\n"); exit(1); }
    Value v = st->data[--st->len];
    return v;
}
