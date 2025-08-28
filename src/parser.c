#include "parser.h"
#include <string.h>
#include <stdarg.h>

void parser_init(ParserContext *ctx) {
    ctx->var_count = 0;
}

void parser_add_var(ParserContext *ctx, const char *name, Ty ty) {
    // Check if variable already exists
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->vars[i].name, name) == 0) {
            ctx->vars[i].ty = ty;
            return;
        }
    }
    
    // Add new variable
    if (ctx->var_count < 256) {
        strncpy(ctx->vars[ctx->var_count].name, name, sizeof(ctx->vars[ctx->var_count].name) - 1);
        ctx->vars[ctx->var_count].name[sizeof(ctx->vars[ctx->var_count].name) - 1] = 0;
        ctx->vars[ctx->var_count].ty = ty;
        ctx->var_count++;
    }
}

Ty parser_get_var_type(ParserContext *ctx, const char *name) {
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->vars[i].name, name) == 0) {
            return ctx->vars[i].ty;
        }
    }
    return TY_UNK;
}

void sb_add(char *out, size_t outsz, int *oi, const char *fmt, ...) {
    if (*oi >= (int)outsz) return;
    va_list ap; 
    va_start(ap, fmt);
    int n = vsnprintf(out + *oi, outsz - *oi, fmt, ap);
    va_end(ap);
    if (n < 0) return;
    *oi += n;
    if (*oi > (int)outsz) *oi = (int)outsz;
}

// Forward declarations
static Ty parse_primary(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_unary(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_mul(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_add(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_rel(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_eq(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);
static Ty parse_and(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx);

static Ty promote_num(Ty a, Ty b, int op_div) {
    if (op_div) return TY_DOUBLE;
    if (a == TY_DOUBLE || b == TY_DOUBLE) return TY_DOUBLE;
    return TY_INT;
}

static Ty parse_primary(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Token t = L->cur;
    
    if (t.kind == T_INT) { 
        sb_add(out, outsz, oi, "%s", t.lex); 
        lex_next(L); 
        return TY_INT; 
    }
    if (t.kind == T_DOUBLE) { 
        sb_add(out, outsz, oi, "%s", t.lex); 
        lex_next(L); 
        return TY_DOUBLE; 
    }
    if (t.kind == T_TRUE) { 
        sb_add(out, outsz, oi, "true"); 
        lex_next(L); 
        return TY_BOOL; 
    }
    if (t.kind == T_FALSE) { 
        sb_add(out, outsz, oi, "false"); 
        lex_next(L); 
        return TY_BOOL; 
    }
    if (t.kind == T_STRING) { 
        sb_add(out, outsz, oi, "%s", t.lex); 
        lex_next(L); 
        return TY_STRING; 
    }
    if (t.kind == T_IDENT) {
        sb_add(out, outsz, oi, "%s", t.lex);
        Ty ty = parser_get_var_type(ctx, t.lex);
        lex_next(L);
        return ty ? ty : TY_UNK;
    }
    if (t.kind == T_LPAREN) { 
        lex_next(L); 
        Ty inner = parse_expr(L, out, outsz, oi, ctx); 
        if (L->cur.kind == T_RPAREN) lex_next(L); 
        return inner; 
    }
    
    return TY_UNK;
}

static Ty parse_unary(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    if (L->cur.kind == T_NOT) { 
        lex_next(L); 
        sb_add(out, outsz, oi, "(!"); 
        (void)parse_unary(L, out, outsz, oi, ctx); 
        sb_add(out, outsz, oi, ")"); 
        return TY_BOOL; 
    }
    if (L->cur.kind == T_PLUS) { 
        lex_next(L); 
        return parse_unary(L, out, outsz, oi, ctx); 
    }
    if (L->cur.kind == T_MINUS) { 
        lex_next(L); 
        sb_add(out, outsz, oi, "(-"); 
        Ty ty = parse_unary(L, out, outsz, oi, ctx); 
        sb_add(out, outsz, oi, ")"); 
        return (ty == TY_DOUBLE) ? TY_DOUBLE : TY_INT; 
    }
    return parse_primary(L, out, outsz, oi, ctx);
}

static Ty parse_mul(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_unary(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_STAR || L->cur.kind == T_SLASH || L->cur.kind == T_PERCENT) {
        TokKind op = L->cur.kind; 
        lex_next(L);
        sb_add(out, outsz, oi, " %s ", op == T_STAR ? "*" : (op == T_SLASH ? "/" : "%"));
        Ty right = parse_unary(L, out, outsz, oi, ctx);
        left = promote_num(left, right, op == T_SLASH);
    }
    return left;
}

static Ty parse_add(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_mul(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_PLUS || L->cur.kind == T_MINUS) {
        TokKind op = L->cur.kind; 
        lex_next(L);
        sb_add(out, outsz, oi, " %s ", op == T_PLUS ? "+" : "-");
        Ty right = parse_mul(L, out, outsz, oi, ctx);
        left = promote_num(left, right, 0);
    }
    return left;
}

static Ty parse_rel(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_add(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_LT || L->cur.kind == T_GT || L->cur.kind == T_LE || L->cur.kind == T_GE) {
        TokKind op = L->cur.kind; 
        lex_next(L);
        const char *sym = op == T_LT ? "<" : op == T_GT ? ">" : op == T_LE ? "<=" : ">=";
        sb_add(out, outsz, oi, " %s ", sym);
        Ty right = parse_add(L, out, outsz, oi, ctx);
        (void)right; 
        left = TY_BOOL;
    }
    return left;
}

static Ty parse_eq(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_rel(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_EQ || L->cur.kind == T_NE) {
        TokKind op = L->cur.kind; 
        lex_next(L);
        sb_add(out, outsz, oi, " %s ", op == T_EQ ? "==" : "!=");
        Ty right = parse_rel(L, out, outsz, oi, ctx);
        (void)right; 
        left = TY_BOOL;
    }
    return left;
}

static Ty parse_and(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_eq(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_AND) { 
        lex_next(L); 
        sb_add(out, outsz, oi, " && "); 
        Ty right = parse_eq(L, out, outsz, oi, ctx); 
        (void)right; 
        left = TY_BOOL; 
    }
    return left;
}

Ty parse_expr(Lexer *L, char *out, size_t outsz, int *oi, ParserContext *ctx) {
    Ty left = parse_and(L, out, outsz, oi, ctx);
    while (L->cur.kind == T_OR) { 
        lex_next(L); 
        sb_add(out, outsz, oi, " || "); 
        Ty right = parse_and(L, out, outsz, oi, ctx); 
        (void)right; 
        left = TY_BOOL; 
    }
    return left;
}
