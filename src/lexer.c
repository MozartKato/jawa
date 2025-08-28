#include "lexer.h"
#include <ctype.h>
#include <string.h>

int is_ident_char(int c) { 
    return isalnum(c) || c=='_'; 
}

void lex_skip_ws(Lexer *L) { 
    while (*L->p && isspace((unsigned char)*L->p)) L->p++; 
}

void lex_next(Lexer *L) {
    lex_skip_ws(L);
    Token t; 
    t.lex[0] = 0; 
    t.kind = T_EOF;
    const char *p = L->p;
    
    if (!*p) { 
        L->cur = t; 
        return; 
    }
    
    char c = *p;
    
    if (isalpha((unsigned char)c) || c=='_') {
        int n = 0; 
        while (is_ident_char((unsigned char)*p) && n < 255) {
            t.lex[n++] = *p++;
        }
        t.lex[n] = 0;
        
        if (strcmp(t.lex, "true") == 0) t.kind = T_TRUE;
        else if (strcmp(t.lex, "false") == 0) t.kind = T_FALSE;
        else t.kind = T_IDENT;
        
    } else if (isdigit((unsigned char)c) || (c=='.' && isdigit((unsigned char)p[1]))) {
        int n = 0; 
        int isdbl = 0;
        while ((isdigit((unsigned char)*p) || *p=='.') && n < 255) { 
            if (*p == '.') isdbl = 1; 
            t.lex[n++] = *p++; 
        }
        t.lex[n] = 0; 
        t.kind = isdbl ? T_DOUBLE : T_INT;
        
    } else if (c == '"') {
        int n = 0; 
        t.lex[n++] = *p++;
        while (*p && *p != '"' && n < 255) {
            if (*p == '\\' && p[1] && n < 254) { 
                t.lex[n++] = *p++; 
                t.lex[n++] = *p++; 
            } else {
                t.lex[n++] = *p++;
            }
        }
        if (*p == '"' && n < 255) t.lex[n++] = *p++;
        t.lex[n] = 0; 
        t.kind = T_STRING;
        
    } else {
        // Two-char operators
        if (p[0]=='<' && p[1]=='=') { t.kind=T_LE; p+=2; }
        else if (p[0]=='>' && p[1]=='=') { t.kind=T_GE; p+=2; }
        else if (p[0]=='=' && p[1]=='=') { t.kind=T_EQ; p+=2; }
        else if (p[0]=='!' && p[1]=='=') { t.kind=T_NE; p+=2; }
        else if (p[0]=='&' && p[1]=='&') { t.kind=T_AND; p+=2; }
        else if (p[0]=='|' && p[1]=='|') { t.kind=T_OR; p+=2; }
        else {
            // Single-char operators
            switch (c) {
                case '(': t.kind=T_LPAREN; p++; break;
                case ')': t.kind=T_RPAREN; p++; break;
                case '+': t.kind=T_PLUS; p++; break;
                case '-': t.kind=T_MINUS; p++; break;
                case '*': t.kind=T_STAR; p++; break;
                case '/': t.kind=T_SLASH; p++; break;
                case '%': t.kind=T_PERCENT; p++; break;
                case '<': t.kind=T_LT; p++; break;
                case '>': t.kind=T_GT; p++; break;
                case '!': t.kind=T_NOT; p++; break;
                default: t.kind=T_EOF; p++; break;
            }
        }
    }
    
    L->p = p; 
    L->cur = t;
}
