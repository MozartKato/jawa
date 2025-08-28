#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "stack.h"
#include "vartable.h"
#include "gc.h"

#include <math.h>
#include "value.h"

void run(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror("open bytecode"); exit(1); }

    char magic[5] = {0};
    fread(magic, 1, 4, f);
    if (strcmp(magic, MAGIC) != 0) {
        fprintf(stderr, "Invalid magic header\n"); exit(1);
    }
    int version = fgetc(f);
    if (version != VERSION) { fprintf(stderr, "Unsupported version\n"); exit(1); }

    Stack st; stack_init(&st);
    VarTable vt; vartable_init(&vt);

    while (1) {
        int op = fgetc(f);
        if (op == EOF) break;

        if (op == OP_PUSH_STR) {
            uint16_t ln = read_u16(f);
            char *buf = malloc(ln + 1);
            if (!buf) { perror("malloc"); exit(1); }
            fread(buf, 1, ln, f);
            buf[ln] = 0;
            StrObj *so = str_new(buf);
            free(buf);
            Value val = make_strobj(so);
            stack_push(&st, val);
        }
        else if (op == OP_PUSH_INT) {
            int32_t iv = read_i32(f);
            Value val = make_int(iv);
            stack_push(&st, val);
        }
        else if (op == OP_PUSH_DOUBLE) {
            double d = read_double(f);
            Value val; val.type = VAL_DOUBLE; val.v.d = d;
            stack_push(&st, val);
        }
        else if (op == OP_PUSH_BOOL) {
            int b = read_bool(f);
            Value val; val.type = VAL_BOOL; val.v.b = b;
            stack_push(&st, val);
        }
        else if (op == OP_STORE) {
            int namelen = fgetc(f);
            char name[256];
            if (namelen <= 0 || namelen >= (int)sizeof(name)) { fprintf(stderr, "Bad name len in STORE\n"); exit(1); }
            fread(name, 1, namelen, f);
            name[namelen] = 0;

            Value v = stack_pop(&st);

            Var *vptr = vartable_find(&vt, name);
            if (!vptr) vptr = vartable_create(&vt, name, 0, v.type);

            if (vptr->val.type == VAL_STR) str_decref(vptr->val.v.s);
            vptr->val = v;
        }
        else if (op == OP_LOAD) {
            int namelen = fgetc(f);
            char name[256];
            if (namelen <= 0 || namelen >= (int)sizeof(name)) { fprintf(stderr, "Bad name len in LOAD\n"); exit(1); }
            fread(name, 1, namelen, f);
            name[namelen] = 0;
            Var *vptr = vartable_find(&vt, name);
            if (!vptr) { fprintf(stderr, "Undefined variable: %s\n", name); exit(1); }
            Value copy = vptr->val;
            if (copy.type == VAL_STR) str_incref(copy.v.s);
            stack_push(&st, copy);
        }
        else if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV || op == OP_MOD) {
            Value b = stack_pop(&st);
            Value a = stack_pop(&st);
            Value res;
            if (a.type == VAL_DOUBLE || b.type == VAL_DOUBLE) {
                double da = (a.type == VAL_DOUBLE) ? a.v.d : a.v.i;
                double db = (b.type == VAL_DOUBLE) ? b.v.d : b.v.i;
                res.type = VAL_DOUBLE;
                switch (op) {
                    case OP_ADD: res.v.d = da + db; break;
                    case OP_SUB: res.v.d = da - db; break;
                    case OP_MUL: res.v.d = da * db; break;
                    case OP_DIV: res.v.d = da / db; break;
                    case OP_MOD: res.v.d = fmod(da, db); break;
                }
            } else {
                res.type = VAL_INT;
                switch (op) {
                    case OP_ADD: res.v.i = a.v.i + b.v.i; break;
                    case OP_SUB: res.v.i = a.v.i - b.v.i; break;
                    case OP_MUL: res.v.i = a.v.i * b.v.i; break;
                    case OP_DIV: res.v.i = a.v.i / b.v.i; break;
                    case OP_MOD: res.v.i = a.v.i % b.v.i; break;
                }
            }
            stack_push(&st, res);
        }
        else if (op == OP_EQ || op == OP_NEQ || op == OP_LT || op == OP_GT || op == OP_LE || op == OP_GE) {
            Value b = stack_pop(&st);
            Value a = stack_pop(&st);
            Value res;
            res.type = VAL_BOOL;
            
            if (a.type == VAL_DOUBLE || b.type == VAL_DOUBLE) {
                double da = (a.type == VAL_DOUBLE) ? a.v.d : a.v.i;
                double db = (b.type == VAL_DOUBLE) ? b.v.d : b.v.i;
                switch (op) {
                    case OP_EQ: res.v.b = (da == db); break;
                    case OP_NEQ: res.v.b = (da != db); break;
                    case OP_LT: res.v.b = (da < db); break;
                    case OP_GT: res.v.b = (da > db); break;
                    case OP_LE: res.v.b = (da <= db); break;
                    case OP_GE: res.v.b = (da >= db); break;
                }
            } else if (a.type == VAL_BOOL && b.type == VAL_BOOL) {
                switch (op) {
                    case OP_EQ: res.v.b = (a.v.b == b.v.b); break;
                    case OP_NEQ: res.v.b = (a.v.b != b.v.b); break;
                    default: res.v.b = 0; break; // bool comparison only supports == and !=
                }
            } else {
                int ia = (a.type == VAL_INT) ? a.v.i : 0;
                int ib = (b.type == VAL_INT) ? b.v.i : 0;
                switch (op) {
                    case OP_EQ: res.v.b = (ia == ib); break;
                    case OP_NEQ: res.v.b = (ia != ib); break;
                    case OP_LT: res.v.b = (ia < ib); break;
                    case OP_GT: res.v.b = (ia > ib); break;
                    case OP_LE: res.v.b = (ia <= ib); break;
                    case OP_GE: res.v.b = (ia >= ib); break;
                }
            }
            stack_push(&st, res);
        }
        else if (op == OP_AND || op == OP_OR) {
            Value b = stack_pop(&st);
            Value a = stack_pop(&st);
            Value res;
            res.type = VAL_BOOL;
            
            int a_true = (a.type == VAL_BOOL) ? a.v.b : 
                         (a.type == VAL_INT) ? (a.v.i != 0) :
                         (a.type == VAL_DOUBLE) ? (a.v.d != 0.0) : 0;
            int b_true = (b.type == VAL_BOOL) ? b.v.b :
                         (b.type == VAL_INT) ? (b.v.i != 0) :
                         (b.type == VAL_DOUBLE) ? (b.v.d != 0.0) : 0;
            
            switch (op) {
                case OP_AND: res.v.b = a_true && b_true; break;
                case OP_OR: res.v.b = a_true || b_true; break;
            }
            stack_push(&st, res);
        }
        else if (op == OP_NOT) {
            Value a = stack_pop(&st);
            Value res;
            res.type = VAL_BOOL;
            
            int a_true = (a.type == VAL_BOOL) ? a.v.b :
                         (a.type == VAL_INT) ? (a.v.i != 0) :
                         (a.type == VAL_DOUBLE) ? (a.v.d != 0.0) : 0;
            res.v.b = !a_true;
            stack_push(&st, res);
        }
        else if (op == OP_JUMP) {
            int32_t offset = read_i32(f);
            fseek(f, offset, SEEK_SET);
        }
        else if (op == OP_JUMP_IF_FALSE) {
            int32_t offset = read_i32(f);
            Value condition = stack_pop(&st);
            
            int is_false = 0;
            if (condition.type == VAL_BOOL) {
                is_false = !condition.v.b;
            } else if (condition.type == VAL_INT) {
                is_false = (condition.v.i == 0);
            } else if (condition.type == VAL_DOUBLE) {
                is_false = (condition.v.d == 0.0);
            }
            
            if (is_false) {
                fseek(f, offset, SEEK_SET);
            }
        }
        else if (op == OP_PRINT) {
            Value v = stack_pop(&st);
            if (v.type == VAL_INT) {
                printf("%d\n", v.v.i);
            } else if (v.type == VAL_DOUBLE) {
                printf("%g\n", v.v.d);
            } else if (v.type == VAL_BOOL) {
                printf("%s\n", v.v.b ? "true" : "false");
            } else if (v.type == VAL_STR) {
                printf("%s\n", v.v.s->s);
                str_decref(v.v.s);
            }
        }
        else if (op == OP_HALT) {
            break;
        }
        else {
            fprintf(stderr, "Unknown opcode %02X\n", op);
            exit(1);
        }
    }

    // cleanup
    stack_free(&st);
    vartable_free(&vt);
    fclose(f);
}
