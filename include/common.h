#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// bytecode header
#define MAGIC "JWBC"
#define VERSION 1


// Opcodes
#define OP_PUSH_STR   0x01
#define OP_PUSH_INT   0x02
#define OP_PUSH_DOUBLE 0x04
#define OP_PUSH_BOOL  0x05

#define OP_ADD        0x20
#define OP_SUB        0x21
#define OP_MUL        0x22
#define OP_DIV        0x23
#define OP_MOD        0x24

#define OP_AND        0x30
#define OP_OR         0x31
#define OP_NOT        0x32

#define OP_EQ         0x40
#define OP_NEQ        0x41
#define OP_LT         0x42
#define OP_GT         0x43
#define OP_LE         0x44
#define OP_GE         0x45

#define OP_STORE      0x10
#define OP_LOAD       0x11
#define OP_PRINT      0x03

#define OP_JUMP       0x50
#define OP_JUMP_IF_FALSE 0x51

#define OP_HALT       0xFF

// IO helpers for double and bool
static inline void write_double(FILE *f, double v) { fwrite(&v, sizeof(v), 1, f); }
static inline double read_double(FILE *f) { double v; fread(&v, sizeof(v), 1, f); return v; }
static inline void write_bool(FILE *f, int v) { fputc(v ? 1 : 0, f); }
static inline int read_bool(FILE *f) { return fgetc(f) ? 1 : 0; }

// IO helpers (use native endianness consistently)
static inline void write_u16(FILE *f, uint16_t v) { fwrite(&v, sizeof(v), 1, f); }
static inline void write_i32(FILE *f, int32_t v) { fwrite(&v, sizeof(v), 1, f); }
static inline uint16_t read_u16(FILE *f) { uint16_t v; fread(&v, sizeof(v), 1, f); return v; }
static inline int32_t read_i32(FILE *f) { int32_t v; fread(&v, sizeof(v), 1, f); return v; }

#endif // COMMON_H
