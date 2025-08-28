#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"

StrObj* str_new(const char *src) {
    StrObj *o = malloc(sizeof(StrObj));
    if (!o) { perror("malloc"); exit(1); }
    o->refcount = 1;
    o->s = strdup(src);
    return o;
}

void str_incref(StrObj *o) {
    if (o) o->refcount++;
}

void str_decref(StrObj *o) {
    if (!o) return;
    o->refcount--;
    if (o->refcount <= 0) {
        free(o->s);
        free(o);
    }
}
