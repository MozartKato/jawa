#include "value.h"

Value make_int(int32_t x) {
    Value val;
    val.type = VAL_INT;
    val.v.i = x;
    return val;
}

Value make_strobj(StrObj *s) {
    Value val;
    val.type = VAL_STR;
    val.v.s = s;
    return val;
}
