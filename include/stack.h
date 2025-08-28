#ifndef STACK_H
#define STACK_H

#include "value.h"
#include <stddef.h>

typedef struct {
    Value *data;
    size_t cap;
    size_t len;
} Stack;

void stack_init(Stack *st);
void stack_free(Stack *st);
void stack_push(Stack *st, Value val);
Value stack_pop(Stack *st);

#endif // STACK_H
