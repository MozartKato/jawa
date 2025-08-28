#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vartable.h"
#include "gc.h"

void vartable_init(VarTable *t) {
    t->len = 0; t->cap = 16;
    t->items = malloc(t->cap * sizeof(Var));
    if (!t->items) { perror("malloc"); exit(1); }
}
void vartable_free(VarTable *t) {
    for (size_t i = 0; i < t->len; ++i) {
        free(t->items[i].name);
        if (t->items[i].val.type == VAL_STR) str_decref(t->items[i].val.v.s);
    }
    free(t->items);
}
Var* vartable_find(VarTable *t, const char *name) {
    for (size_t i = 0; i < t->len; ++i) {
        if (strcmp(t->items[i].name, name) == 0) return &t->items[i];
    }
    return NULL;
}
Var* vartable_create(VarTable *t, const char *name, int scope, ValType declared_type) {
    if (t->len == t->cap) {
        t->cap *= 2;
        t->items = realloc(t->items, t->cap * sizeof(Var));
        if (!t->items) { perror("realloc"); exit(1); }
    }
    t->items[t->len].name = strdup(name);
    t->items[t->len].val.type = declared_type;
    // default value
    switch (declared_type) {
        case VAL_INT: t->items[t->len].val.v.i = 0; break;
        case VAL_DOUBLE: t->items[t->len].val.v.d = 0.0; break;
        case VAL_BOOL: t->items[t->len].val.v.b = 0; break;
        case VAL_STR: t->items[t->len].val.v.s = NULL; break;
    }
    t->items[t->len].scope = scope;
    t->items[t->len].declared_type = declared_type;
    return &t->items[t->len++];
}
