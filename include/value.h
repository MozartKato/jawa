#ifndef VALUE_H
#define VALUE_H

#include "gc.h"
#include <stdint.h>

typedef enum {
    VAL_INT,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_STR
} ValType;

typedef struct {
    ValType type;
    union {
        int32_t i;
        double d;
        int b;
        StrObj *s;
    } v;
} Value;

Value make_int(int32_t x);
Value make_strobj(StrObj *s);

#endif // VALUE_H
