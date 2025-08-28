#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdio.h>

// Type system
typedef enum { 
    TY_UNK=0, TY_INT, TY_DOUBLE, TY_BOOL, TY_STRING, TY_ARRAY, TY_MAP
} Ty;

// Variable tracking
typedef struct { 
    char name[128]; 
    Ty ty; 
} VarInfo;

// Parser context
typedef struct {
    VarInfo vars[256];
    int var_count;
} ParserContext;

// Parser functions
void parser_init(ParserContext *ctx);
void parser_add_var(ParserContext *ctx, const char *name, Ty ty);
Ty parser_get_var_type(ParserContext *ctx, const char *name);

// Expression parsing
Ty parse_expr(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
Ty parse_method_call(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx, const char *object_name);
Ty parse_array_literal(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
Ty parse_map_literal(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);

// Utility functions
void sb_add(char *out, size_t outsz, int *oi, const char *fmt, ...);

#endif // PARSER_H
