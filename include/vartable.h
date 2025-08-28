#ifndef VARTABLE_H
#define VARTABLE_H

#include "value.h"
#include <stddef.h>

typedef struct {
    char *name;
    Value val;
    enum { SCOPE_VAR, SCOPE_LET, SCOPE_CONST } scope;
    ValType declared_type;
} Var;

typedef struct {
    Var *items;
    size_t len;
    size_t cap;
} VarTable;

void vartable_init(VarTable *t);
void vartable_free(VarTable *t);
Var* vartable_find(VarTable *t, const char *name);
Var* vartable_create(VarTable *t, const char *name, int scope, ValType declared_type);

#endif // VARTABLE_H
