#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum { TY_UNK=0, TY_INT, TY_DOUBLE, TY_BOOL, TY_STRING } Ty;
typedef struct { char name[128]; Ty ty; } VarInfo;
static VarInfo g_vars[256];
static int g_var_count = 0;

static void vars_reset() { g_var_count = 0; }
static void vars_set(const char *name, Ty ty) {
    for (int i=0;i<g_var_count;i++) if (strcmp(g_vars[i].name, name)==0) { g_vars[i].ty = ty; return; }
    if (g_var_count < (int)(sizeof(g_vars)/sizeof(g_vars[0]))) {
        strncpy(g_vars[g_var_count].name, name, sizeof(g_vars[g_var_count].name)-1);
        g_vars[g_var_count].name[sizeof(g_vars[g_var_count].name)-1] = 0;
        g_vars[g_var_count].ty = ty;
        g_var_count++;
    }
}
static Ty vars_get(const char *name) {
    for (int i=0;i<g_var_count;i++) if (strcmp(g_vars[i].name, name)==0) return g_vars[i].ty;
    return TY_UNK;
}

// removed old helper is_single_identifier; not needed with the new parser

// Track if we're inside an if block for proper else handling
static int g_if_depth = 0;

static void write_preamble(FILE *c) {
    fputs("#include <stdio.h>\n#include <stdint.h>\n#include <stdbool.h>\n\n", c);
}

static int is_ident_char(int c) { return isalnum(c) || c=='_'; }

// --- Tokenizer ---
typedef enum {
    T_EOF=0, T_IDENT, T_INT, T_DOUBLE, T_STRING, T_TRUE, T_FALSE,
    T_LPAREN, T_RPAREN,
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT,
    T_LT, T_GT, T_LE, T_GE, T_EQ, T_NE,
    T_AND, T_OR, T_NOT
} TokKind;

typedef struct { TokKind kind; char lex[256]; } Token;

typedef struct { const char *p; Token cur; } Lexer;

static void lex_skip_ws(Lexer *L) { while (*L->p && isspace((unsigned char)*L->p)) L->p++; }
static void lex_next(Lexer *L) {
    lex_skip_ws(L);
    Token t; t.lex[0]=0; t.kind=T_EOF;
    const char *p=L->p;
    if (!*p) { L->cur=t; return; }
    char c=*p;
    if (isalpha((unsigned char)c) || c=='_') {
        int n=0; while (is_ident_char((unsigned char)*p) && n<255) t.lex[n++]=*p++;
        t.lex[n]=0;
        if (strcmp(t.lex, "true")==0) t.kind=T_TRUE;
        else if (strcmp(t.lex, "false")==0) t.kind=T_FALSE;
        else t.kind=T_IDENT;
    } else if (isdigit((unsigned char)c) || (c=='.' && isdigit((unsigned char)p[1]))) {
        int n=0; int isdbl=0;
        while ((isdigit((unsigned char)*p) || *p=='.') && n<255) { if (*p=='.') isdbl=1; t.lex[n++]=*p++; }
        t.lex[n]=0; t.kind = isdbl ? T_DOUBLE : T_INT;
    } else if (c=='"') {
        int n=0; t.lex[n++]=*p++;
        while (*p && *p!='"' && n<255) {
            if (*p=='\\' && p[1] && n<254) { t.lex[n++]=*p++; t.lex[n++]=*p++; }
            else t.lex[n++]=*p++;
        }
        if (*p=='"' && n<255) t.lex[n++]=*p++;
        t.lex[n]=0; t.kind=T_STRING;
    } else {
        // two-char operators
        if (p[0]=='<' && p[1]=='=') { t.kind=T_LE; p+=2; }
        else if (p[0]=='>' && p[1]=='=') { t.kind=T_GE; p+=2; }
        else if (p[0]=='=' && p[1]=='=') { t.kind=T_EQ; p+=2; }
        else if (p[0]=='!' && p[1]=='=') { t.kind=T_NE; p+=2; }
        else if (p[0]=='&' && p[1]=='&') { t.kind=T_AND; p+=2; }
        else if (p[0]=='|' && p[1]=='|') { t.kind=T_OR; p+=2; }
        else {
            // single-char
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
    L->p = p; L->cur = t;
}

// string builder
static void sb_add(char *out, size_t outsz, int *oi, const char *fmt, ...) {
    if (*oi >= (int)outsz) return;
    va_list ap; va_start(ap, fmt);
    int n = vsnprintf(out + *oi, outsz - *oi, fmt, ap);
    va_end(ap);
    if (n < 0) return;
    *oi += n;
    if (*oi > (int)outsz) *oi = (int)outsz;
}

// Forward decls
static Ty parse_expr(Lexer *L, char *out, size_t outsz, int *oi);

static Ty parse_primary(Lexer *L, char *out, size_t outsz, int *oi) {
    Token t = L->cur;
    if (t.kind==T_INT) { sb_add(out,outsz,oi, "%s", t.lex); lex_next(L); return TY_INT; }
    if (t.kind==T_DOUBLE) { sb_add(out,outsz,oi, "%s", t.lex); lex_next(L); return TY_DOUBLE; }
    if (t.kind==T_TRUE) { sb_add(out,outsz,oi, "true"); lex_next(L); return TY_BOOL; }
    if (t.kind==T_FALSE) { sb_add(out,outsz,oi, "false"); lex_next(L); return TY_BOOL; }
    if (t.kind==T_STRING) { sb_add(out,outsz,oi, "%s", t.lex); lex_next(L); return TY_STRING; }
    if (t.kind==T_IDENT) {
        sb_add(out,outsz,oi, "%s", t.lex);
        Ty ty = vars_get(t.lex);
        lex_next(L);
        return ty ? ty : TY_UNK;
    }
    if (t.kind==T_LPAREN) { lex_next(L); Ty inner = parse_expr(L,out,outsz,oi); if (L->cur.kind==T_RPAREN) lex_next(L); return inner; }
    // fallback
    return TY_UNK;
}

static Ty parse_unary(Lexer *L, char *out, size_t outsz, int *oi) {
    if (L->cur.kind==T_NOT) { lex_next(L); sb_add(out,outsz,oi, "(!"); (void)parse_unary(L,out,outsz,oi); sb_add(out,outsz,oi, ")"); return TY_BOOL; }
    if (L->cur.kind==T_PLUS) { lex_next(L); return parse_unary(L,out,outsz,oi); }
    if (L->cur.kind==T_MINUS) { lex_next(L); sb_add(out,outsz,oi, "(-"); Ty ty = parse_unary(L,out,outsz,oi); sb_add(out,outsz,oi, ")"); return (ty==TY_DOUBLE)?TY_DOUBLE:TY_INT; }
    return parse_primary(L,out,outsz,oi);
}

static Ty promote_num(Ty a, Ty b, int op_div) {
    if (op_div) return TY_DOUBLE;
    if (a==TY_DOUBLE || b==TY_DOUBLE) return TY_DOUBLE;
    return TY_INT;
}

static Ty parse_mul(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_unary(L,out,outsz,oi);
    while (L->cur.kind==T_STAR || L->cur.kind==T_SLASH || L->cur.kind==T_PERCENT) {
        TokKind op = L->cur.kind; lex_next(L);
        sb_add(out,outsz,oi, " %s ", op==T_STAR?"*":(op==T_SLASH?"/":"%"));
        Ty right = parse_unary(L,out,outsz,oi);
        left = promote_num(left,right, op==T_SLASH);
    }
    return left;
}

static Ty parse_add(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_mul(L,out,outsz,oi);
    while (L->cur.kind==T_PLUS || L->cur.kind==T_MINUS) {
        TokKind op = L->cur.kind; lex_next(L);
        sb_add(out,outsz,oi, " %s ", op==T_PLUS?"+":"-");
        Ty right = parse_mul(L,out,outsz,oi);
        left = promote_num(left,right, 0);
    }
    return left;
}

static Ty parse_rel(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_add(L,out,outsz,oi);
    while (L->cur.kind==T_LT || L->cur.kind==T_GT || L->cur.kind==T_LE || L->cur.kind==T_GE) {
        TokKind op = L->cur.kind; lex_next(L);
        const char *sym = op==T_LT?"<": op==T_GT?">": op==T_LE?"<=":">=";
        sb_add(out,outsz,oi, " %s ", sym);
        Ty right = parse_add(L,out,outsz,oi);
        (void)right; left = TY_BOOL;
    }
    return left;
}

static Ty parse_eq(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_rel(L,out,outsz,oi);
    while (L->cur.kind==T_EQ || L->cur.kind==T_NE) {
        TokKind op = L->cur.kind; lex_next(L);
        sb_add(out,outsz,oi, " %s ", op==T_EQ?"==":"!=");
        Ty right = parse_rel(L,out,outsz,oi);
        (void)right; left = TY_BOOL;
    }
    return left;
}

static Ty parse_and(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_eq(L,out,outsz,oi);
    while (L->cur.kind==T_AND) { lex_next(L); sb_add(out,outsz,oi, " && "); Ty right = parse_eq(L,out,outsz,oi); (void)right; left = TY_BOOL; }
    return left;
}

static Ty parse_expr(Lexer *L, char *out, size_t outsz, int *oi) {
    Ty left = parse_and(L,out,outsz,oi);
    while (L->cur.kind==T_OR) { lex_next(L); sb_add(out,outsz,oi, " || "); Ty right = parse_and(L,out,outsz,oi); (void)right; left = TY_BOOL; }
    return left;
}

int build_native(const char *srcPath, const char *outPath) {
    FILE *in = fopen(srcPath, "r");
    if (!in) { perror("open src"); return 1; }
    char tmpc[FILENAME_MAX];
    snprintf(tmpc, sizeof(tmpc), "%s.c", outPath);
    FILE *c = fopen(tmpc, "w");
    if (!c) { perror("open c out"); fclose(in); return 2; }

    write_preamble(c);
    fputs("int main(){\n", c);

    char line[512];
    vars_reset();
    g_if_depth = 0;
    while (fgets(line, sizeof(line), in)) {
        // Strip newline
        size_t len = strlen(line); if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        const char *p = line; while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;

        if (strncmp(p, "owahi ", 6) == 0 || strncmp(p, "cendhak ", 8) == 0 || strncmp(p, "ajek ", 5) == 0 || 
            strncmp(p, "var ", 4) == 0 || strncmp(p, "let ", 4) == 0) {
            // Parse: kind name [: type] = expr
            const char *q = p;
            if (strncmp(p, "owahi ", 6) == 0) q = p + 6;
            else if (strncmp(p, "cendhak ", 8) == 0) q = p + 8;
            else if (strncmp(p, "ajek ", 5) == 0) q = p + 5;
            else if (strncmp(p, "var ", 4) == 0) q = p + 4;
            else if (strncmp(p, "let ", 4) == 0) q = p + 4;
            while (*q==' ') q++;
            char name[128]; int ni=0; while (is_ident_char((unsigned char)*q) && ni<127) name[ni++]=*q++;
            name[ni]=0; while (*q==' ') q++;
            char ctype[32] = {0};
            if (*q==':') { q++; while (*q==' ') q++; int ti=0; while (is_ident_char((unsigned char)*q) && ti<31) ctype[ti++]=*q++; ctype[ti]=0; while (*q==' ') q++; }
            if (*q=='=') q++;
            while (*q==' ') q++;
            // Map types to C types (default double)
            const char *mapped = "double";
            Ty vty = TY_DOUBLE;
            if (ctype[0]) {
                if (strcmp(ctype, "int")==0) { mapped = "int"; vty = TY_INT; }
                else if (strcmp(ctype, "double")==0) { mapped = "double"; vty = TY_DOUBLE; }
                else if (strcmp(ctype, "bool")==0) { mapped = "bool"; vty = TY_BOOL; }
                else if (strcmp(ctype, "string")==0) { mapped = "const char*"; vty = TY_STRING; }
            }
            vars_set(name, vty);
            fprintf(c, "%s %s = ", mapped, name);
            char ebuf[4096]; int oi=0; ebuf[0]=0;
            Lexer L = { .p = q };
            lex_next(&L);
            (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi);
            fputs(ebuf, c);
            fputs(";\n", c);
            continue;
        }

        if (strncmp(p, "cithak", 6) == 0 && isspace((unsigned char)p[6])) {
            const char *q = p + 6; while (*q && isspace((unsigned char)*q)) q++;
            // Determine if printing string/bool or numeric
            if (*q=='\"') {
                char ebuf[4096]; int oi=0; ebuf[0]=0; Lexer L={.p=q}; lex_next(&L); Ty et=parse_expr(&L, ebuf, sizeof(ebuf), &oi); (void)et;
                fprintf(c, "printf(\"%%s\\n\", %s);\n", ebuf);
            } else {
                // For bool, print true/false
                char ebuf[4096]; int oi=0; ebuf[0]=0; Lexer L={.p=q}; lex_next(&L); Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                if (et == TY_STRING) {
                    fprintf(c, "printf(\"%%s\\n\", %s);\n", ebuf);
                } else if (et == TY_BOOL) {
                    fprintf(c, "printf(\"%%s\\n\", (%s) ? \"true\" : \"false\");\n", ebuf);
                } else {
                    fprintf(c, "printf(\"%%g\\n\", (double)(%s));\n", ebuf);
                }
            }
            continue;
        }

        if (strncmp(p, "yen", 3) == 0 && isspace((unsigned char)p[3])) {
            const char *q = p + 3; while (*q && isspace((unsigned char)*q)) q++;
            // Find the opening brace
            const char *brace_pos = strchr(q, '{');
            if (brace_pos) {
                // Extract condition (everything before {)
                int cond_len = (int)(brace_pos - q);
                char cond_buf[512]; 
                if (cond_len < (int)sizeof(cond_buf)) {
                    strncpy(cond_buf, q, cond_len);
                    cond_buf[cond_len] = 0;
                    
                    // Remove trailing spaces
                    char *end = cond_buf + cond_len - 1;
                    while (end > cond_buf && isspace((unsigned char)*end)) *end-- = 0;
                    
                    // Parse condition expression
                    char ebuf[4096]; int oi=0; ebuf[0]=0; 
                    Lexer L={.p=cond_buf}; lex_next(&L); 
                    Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                    
                    fprintf(c, "if (");
                    if (et == TY_BOOL) {
                        fprintf(c, "%s", ebuf);
                    } else {
                        fprintf(c, "(%s)", ebuf); // treat non-zero as true
                    }
                    fputs(") {\n", c);
                    g_if_depth++;
                }
            }
            continue;
        }

        if (strncmp(p, "liyane", 6) == 0) {
            // Check if this line has { at the end
            const char *q = p + 6;
            while (*q && isspace((unsigned char)*q)) q++;
            if (*q == '{' || *q == 0) {
                if (g_if_depth > 0) {
                    fputs("} else {\n", c);
                } else {
                    fputs("else {\n", c);
                    g_if_depth++;
                }
            }
            continue;
        }

        if (strcmp(p, "}") == 0 || strstr(p, "}")) {
            fputs("}\n", c);
            if (g_if_depth > 0) g_if_depth--;
            continue;
        }

        // handle menawa (while loop)
        if (strncmp(p, "menawa", 6) == 0 && isspace((unsigned char)p[6])) {
            const char *q = p + 6; while (*q && isspace((unsigned char)*q)) q++;
            // Find the opening brace
            const char *brace_pos = strchr(q, '{');
            if (brace_pos) {
                // Extract condition (everything before {)
                int cond_len = (int)(brace_pos - q);
                char cond_buf[512]; 
                if (cond_len < (int)sizeof(cond_buf)) {
                    strncpy(cond_buf, q, cond_len);
                    cond_buf[cond_len] = 0;
                    
                    // Remove trailing spaces and parentheses
                    char *end = cond_buf + cond_len - 1;
                    while (end > cond_buf && isspace((unsigned char)*end)) *end-- = 0;
                    if (cond_buf[0] == '(' && end > cond_buf && *end == ')') {
                        memmove(cond_buf, cond_buf + 1, end - cond_buf);
                        cond_buf[end - cond_buf - 1] = 0;
                    }
                    
                    // Parse condition expression
                    char ebuf[4096]; int oi=0; ebuf[0]=0; 
                    Lexer L={.p=cond_buf}; lex_next(&L); 
                    Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                    
                    fprintf(c, "while (");
                    if (et == TY_BOOL) {
                        fprintf(c, "%s", ebuf);
                    } else {
                        fprintf(c, "(%s)", ebuf); // treat non-zero as true
                    }
                    fputs(") {\n", c);
                }
            }
            continue;
        }

        // handle ngulang (for loop)
        if (strncmp(p, "ngulang", 7) == 0 && isspace((unsigned char)p[7])) {
            const char *q = p + 7; while (*q && isspace((unsigned char)*q)) q++;
            // Find the opening brace
            const char *brace_pos = strchr(q, '{');
            if (brace_pos) {
                // Extract for statement (everything before {)
                int for_len = (int)(brace_pos - q);
                char for_buf[512]; 
                if (for_len < (int)sizeof(for_buf)) {
                    strncpy(for_buf, q, for_len);
                    for_buf[for_len] = 0;
                    
                    // Remove outer parentheses if present
                    char *start = for_buf;
                    while (*start && isspace((unsigned char)*start)) start++;
                    if (*start == '(') {
                        char *end = for_buf + for_len - 1;
                        while (end > start && isspace((unsigned char)*end)) end--;
                        if (*end == ')') {
                            start++;
                            *end = 0;
                        }
                    }
                    
                    // Parse for components: init; condition; increment
                    char *semi1 = strchr(start, ';');
                    char *semi2 = semi1 ? strchr(semi1 + 1, ';') : NULL;
                    
                    fputs("for (", c);
                    
                    if (semi1 && semi2) {
                        // init statement
                        *semi1 = 0;
                        if (strlen(start) > 0) {
                            char *eq = strchr(start, '=');
                            if (eq) {
                                *eq = 0;
                                char *var_name = start;
                                while (*var_name && isspace((unsigned char)*var_name)) var_name++;
                                char *var_end = eq - 1;
                                while (var_end > var_name && isspace((unsigned char)*var_end)) *var_end-- = 0;
                                
                                char *val_start = eq + 1;
                                while (*val_start && isspace((unsigned char)*val_start)) val_start++;
                                
                                char ebuf[1024]; int oi=0; ebuf[0]=0;
                                Lexer L={.p=val_start}; lex_next(&L);
                                Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                                
                                if (et == TY_DOUBLE || et == TY_UNK) {
                                    vars_set(var_name, TY_DOUBLE);
                                    fprintf(c, "double %s = %s", var_name, ebuf);
                                } else {
                                    vars_set(var_name, TY_INT);
                                    fprintf(c, "int %s = %s", var_name, ebuf);
                                }
                            }
                        }
                        
                        fputs("; ", c);
                        
                        // condition
                        char *cond_start = semi1 + 1;
                        *semi2 = 0;
                        while (*cond_start && isspace((unsigned char)*cond_start)) cond_start++;
                        if (strlen(cond_start) > 0) {
                            char ebuf[1024]; int oi=0; ebuf[0]=0;
                            Lexer L={.p=cond_start}; lex_next(&L);
                            Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                            (void)et; // Suppress unused variable warning
                            fprintf(c, "%s", ebuf);
                        }
                        
                        fputs("; ", c);
                        
                        // increment
                        char *incr_start = semi2 + 1;
                        while (*incr_start && isspace((unsigned char)*incr_start)) incr_start++;
                        if (strlen(incr_start) > 0) {
                            char *eq = strchr(incr_start, '=');
                            if (eq) {
                                *eq = 0;
                                char *var_name = incr_start;
                                while (*var_name && isspace((unsigned char)*var_name)) var_name++;
                                char *var_end = eq - 1;
                                while (var_end > var_name && isspace((unsigned char)*var_end)) *var_end-- = 0;
                                
                                char *val_start = eq + 1;
                                while (*val_start && isspace((unsigned char)*val_start)) val_start++;
                                
                                char ebuf[1024]; int oi=0; ebuf[0]=0;
                                Lexer L={.p=val_start}; lex_next(&L);
                                Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                                (void)et; // Suppress unused variable warning
                                
                                fprintf(c, "%s = %s", var_name, ebuf);
                            }
                        }
                    }
                    
                    fputs(") {\n", c);
                }
            }
            continue;
        }

        // Handle opening braces
        if (strcmp(p, "{") == 0) {
            // Just a standalone opening brace, skip it since it was handled by yen/liyane
            continue;
        }

        // Handle assignment: var = expr
        char *eq_pos = strchr(p, '=');
        if (eq_pos) {
            // Extract variable name
            char var_name[128];
            int name_len = (int)(eq_pos - p);
            if (name_len > 0 && name_len < 127) {
                strncpy(var_name, p, name_len);
                var_name[name_len] = 0;
                
                // Trim spaces from variable name
                char *name_start = var_name;
                while (*name_start && isspace((unsigned char)*name_start)) name_start++;
                char *name_end = var_name + strlen(var_name) - 1;
                while (name_end > name_start && isspace((unsigned char)*name_end)) *name_end-- = 0;
                
                // Check if it's a valid identifier (not a declaration)
                int is_decl = (strncmp(p, "owahi ", 6) == 0 || strncmp(p, "cendhak ", 8) == 0 || 
                              strncmp(p, "ajek ", 5) == 0 || strncmp(p, "var ", 4) == 0 || 
                              strncmp(p, "let ", 4) == 0);
                if (!is_decl && strlen(name_start) > 0) {
                    // Parse expression after =
                    char *expr_start = eq_pos + 1;
                    while (*expr_start && isspace((unsigned char)*expr_start)) expr_start++;
                    
                    if (strlen(expr_start) > 0) {
                        char ebuf[1024];
                        int oi = 0;
                        ebuf[0] = 0;
                        
                        Lexer L = {.p = expr_start};
                        lex_next(&L);
                        (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi);
                        
                        fprintf(c, "%s = %s;\n", name_start, ebuf);
                        continue;
                    }
                }
            }
        }

        // Unknown line: ignore for now
    }

    fputs("return 0;\n}\n", c);
    fclose(in); fclose(c);

    char cmd[8192];
    snprintf(cmd, sizeof(cmd), "cc -O2 -std=c11 -o '%s' '%s'", outPath, tmpc);
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "Failed to build native binary (rc=%d)\n", rc);
        return 3;
    }
    return 0;
}
