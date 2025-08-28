#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

// Token types
typedef enum {
    T_EOF=0, T_IDENT, T_INT, T_DOUBLE, T_STRING, T_TRUE, T_FALSE,
    T_LPAREN, T_RPAREN,
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT,
    T_LT, T_GT, T_LE, T_GE, T_EQ, T_NE,
    T_AND, T_OR, T_NOT
} TokKind;

typedef struct { 
    TokKind kind; 
    char lex[256]; 
} Token;

typedef struct { 
    const char *p; 
    Token cur; 
} Lexer;

// Lexer functions
void lex_skip_ws(Lexer *L);
void lex_next(Lexer *L);
int is_ident_char(int c);

#endif // LEXER_H
