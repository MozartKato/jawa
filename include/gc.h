#ifndef GC_H
#define GC_H

typedef struct StrObj {
    int refcount;
    char *s;
} StrObj;

StrObj* str_new(const char *src);
void str_incref(StrObj *o);
void str_decref(StrObj *o);

#endif // GC_H
