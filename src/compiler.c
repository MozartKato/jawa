
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"


static int is_identifier_char(int c) {
    return isalnum(c) || c == '_' ;
}

static void write_name(FILE *f, const char *name) {
    uint8_t len = (uint8_t)strlen(name);
    fputc(len, f);
    fwrite(name, 1, len, f);
}

// Simple error reporter with file and line number
static const char *g_current_src = NULL;
static void error_at(int lineno, const char *fmt, ...) {
    fprintf(stderr, "%s:%d: error: ", g_current_src ? g_current_src : "<input>", lineno);
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(1);
}

// Forward declarations for expression parsing
static void emit_number_or_name(FILE *fout, char **pp);
static void emit_expression(FILE *fout, char **pp);
static void emit_comparison(FILE *fout, char **pp);
static void emit_additive(FILE *fout, char **pp);
static void emit_multiplicative(FILE *fout, char **pp);
static void emit_primary_expr(FILE *fout, char **pp);

// Helper function for cithak statement
static void emit_cithak_statement(FILE *fout, char **pp) {
    char *p = *pp;
    if (*p == '"' || *p == '\'') {
        char quote = *p++;
        char buf[512]; int bl = 0;
        while (*p && *p != quote && bl < (int)sizeof(buf)-1) {
            if (*p == '\\' && *(p+1)) {
                p++;
                if (*p == 'n') buf[bl++] = '\n';
                else if (*p == 't') buf[bl++] = '\t';
                else if (*p == '\\') buf[bl++] = '\\';
                else buf[bl++] = *p;
                p++;
            } else {
                buf[bl++] = *p++;
            }
        }
        buf[bl] = 0;
        fputc(OP_PUSH_STR, fout);
        write_u16(fout, (uint16_t)bl);
        fwrite(buf, 1, bl, fout);
        fputc(OP_PRINT, fout);
        *pp = p;
    } else if (isalpha((unsigned char)*p) || *p == '_') {
        char name[128]; int ni = 0;
        while (is_identifier_char((unsigned char)*p) && ni < (int)sizeof(name)-1) name[ni++] = *p++;
        name[ni] = 0;
        fputc(OP_LOAD, fout);
        write_name(fout, name);
        fputc(OP_PRINT, fout);
        *pp = p;
    } else {
        // expression
        emit_expression(fout, pp);
        fputc(OP_PRINT, fout);
    }
}

// Fungsi bantu: skip whitespace
static void skip_ws(char **pp) {
    while (**pp == ' ' || **pp == '\t') (*pp)++;
}

// Parsing dan emit ekspresi aritmatika sederhana (operand1 op operand2)
// Mendukung: identifier, integer, double, + - * / %, dan parentheses
static void emit_number_or_name(FILE *fout, char **pp) {
    char *p = *pp;
    skip_ws(&p);
    
    if (*p == '(') {
        // Handle parentheses
        p++;
        skip_ws(&p);
        emit_expression(fout, &p);
        skip_ws(&p);
        if (*p == ')') p++;
        *pp = p;
        return;
    }
    
    if (isalpha((unsigned char)*p) || *p == '_') {
        char name[128]; int ni = 0;
        while (is_identifier_char((unsigned char)*p) && ni < (int)sizeof(name)-1) name[ni++] = *p++;
        name[ni] = 0;
        fputc(OP_LOAD, fout);
        write_name(fout, name);
        *pp = p; return;
    }
    if (isdigit((unsigned char)*p) || *p == '+' || *p == '-') {
        char numbuf[64]; int ni2 = 0; int is_double = 0;
        while (((*p == '+' || *p == '-' || isdigit((unsigned char)*p) || *p == '.') && ni2 < (int)sizeof(numbuf)-1)) {
            if (*p == '.') is_double = 1;
            numbuf[ni2++] = *p++;
        }
        numbuf[ni2] = 0;
        if (is_double) { double val = atof(numbuf); fputc(OP_PUSH_DOUBLE, fout); write_double(fout, val); }
        else { int32_t val = atoi(numbuf); fputc(OP_PUSH_INT, fout); write_i32(fout, val); }
        *pp = p; return;
    }
    error_at(0, "Unsupported expression operand");
}

static void emit_expression(FILE *fout, char **pp) {
    // Simple recursive descent for precedence: comparison (lowest) -> + - -> * / %
    emit_comparison(fout, pp);
}

static void emit_comparison(FILE *fout, char **pp) {
    emit_additive(fout, pp);
    char *p = *pp;
    while (1) {
        skip_ws(&p);
        if (strncmp(p, ">=", 2) == 0) {
            p += 2;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_GE, fout);
        } else if (strncmp(p, "<=", 2) == 0) {
            p += 2;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_LE, fout);
        } else if (strncmp(p, "==", 2) == 0) {
            p += 2;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_EQ, fout);
        } else if (strncmp(p, "!=", 2) == 0) {
            p += 2;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_NEQ, fout);
        } else if (*p == '>') {
            p++;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_GT, fout);
        } else if (*p == '<') {
            p++;
            skip_ws(&p);
            emit_additive(fout, &p);
            fputc(OP_LT, fout);
        } else {
            break;
        }
    }
    *pp = p;
}

static void emit_additive(FILE *fout, char **pp) {
    emit_multiplicative(fout, pp);
    char *p = *pp;
    while (1) {
        skip_ws(&p);
        if (*p != '+' && *p != '-') break;
        char op = *p++;
        skip_ws(&p);
        emit_multiplicative(fout, &p);
        fputc(op == '+' ? OP_ADD : OP_SUB, fout);
    }
    *pp = p;
}

static void emit_multiplicative(FILE *fout, char **pp) {
    emit_primary_expr(fout, pp);
    char *p = *pp;
    while (1) {
        skip_ws(&p);
        if (*p != '*' && *p != '/' && *p != '%') break;
        char op = *p++;
        skip_ws(&p);
        emit_primary_expr(fout, &p);
        char opcode = (op == '*') ? OP_MUL : (op == '/') ? OP_DIV : OP_MOD;
        fputc(opcode, fout);
    }
    *pp = p;
}

static void emit_primary_expr(FILE *fout, char **pp) {
    emit_number_or_name(fout, pp);
}

#include <string.h>
#include "compiler.h"
#include "value.h"

void compile(const char *src, const char *out) {
    g_current_src = src;
    FILE *fin = fopen(src, "r");
    if (!fin) { perror("open src"); exit(1); }
    FILE *fout = fopen(out, "wb");
    if (!fout) { perror("open out"); fclose(fin); exit(1); }

    // Header
    fwrite(MAGIC, 1, 4, fout);
    fputc(VERSION, fout);

    char line[1024];
    int lineno = 0;
    while (fgets(line, sizeof(line), fin)) {
        lineno++;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++; // ltrim

        // skip empty or comment
        if (*p == '\0' || *p == '\n' || *p == '#') continue;

        // handle owahi/cendhak/ajek: [owahi|cendhak|ajek] <name> [:<type>] = <literal>
    // int scope = -1; // tidak dipakai
        if ((strncmp(p, "cendhak", 7) == 0 && isspace((unsigned char)p[7])) ||
            (strncmp(p, "owahi", 5) == 0 && isspace((unsigned char)p[5])) ||
            (strncmp(p, "ajek", 4) == 0 && isspace((unsigned char)p[4]))) {
            if (strncmp(p, "cendhak", 7) == 0) { p += 7; }
            else if (strncmp(p, "owahi", 5) == 0) { p += 5; }
            else if (strncmp(p, "ajek", 4) == 0) { p += 4; }
            while (*p == ' ' || *p == '\t') p++;
            // read identifier
            char name[128]; int ni = 0;
            if (!isalpha((unsigned char)*p) && *p != '_') {
                fprintf(stderr, "[%d] Invalid identifier in declaration\n", lineno); exit(1);
            }
            while (is_identifier_char((unsigned char)*p) && ni < (int)sizeof(name)-1) name[ni++] = *p++;
            name[ni] = 0;
            while (*p == ' ' || *p == '\t') p++;
            // optional type annotation
            ValType declared_type = VAL_INT;
            if (*p == ':') {
                p++;
                while (*p == ' ' || *p == '\t') p++;
                if (strncmp(p, "int", 3) == 0) { declared_type = VAL_INT; p += 3; }
                else if (strncmp(p, "double", 6) == 0) { declared_type = VAL_DOUBLE; p += 6; }
                else if (strncmp(p, "bool", 4) == 0) { declared_type = VAL_BOOL; p += 4; }
                else if (strncmp(p, "string", 6) == 0) { declared_type = VAL_STR; p += 6; }
                else { error_at(lineno, "Unknown type in declaration"); }
                while (*p == ' ' || *p == '\t') p++;
            }
            if (*p != '=') { error_at(lineno, "Expected '=' in declaration"); }
            p++; while (*p == ' ' || *p == '\t') p++;

            // literal: string, int, double, bool, atau ekspresi aritmatika sederhana
            if (*p == '"' || *p == '\'') {
                char quote = *p++;
                char buf[512]; int bl = 0;
                while (*p && *p != quote && bl < (int)sizeof(buf)-1) {
                    if (*p == '\\' && *(p+1)) {
                        p++;
                        if (*p == 'n') buf[bl++] = '\n';
                        else if (*p == 't') buf[bl++] = '\t';
                        else if (*p == '\\') buf[bl++] = '\\';
                        else buf[bl++] = *p;
                        p++;
                    } else {
                        buf[bl++] = *p++;
                    }
                }
                buf[bl] = 0;
                // emit push_str, store name
                fputc(OP_PUSH_STR, fout);
                write_u16(fout, (uint16_t)bl);
                fwrite(buf, 1, bl, fout);
                fputc(OP_STORE, fout);
                write_name(fout, name);
            } else if (strchr(p, '+') || strchr(p, '-') || strchr(p, '*') || strchr(p, '/') || strchr(p, '%')) {
                // ekspresi aritmatika sederhana
                emit_expression(fout, &p);
                fputc(OP_STORE, fout);
                write_name(fout, name);
            } else if (declared_type == VAL_DOUBLE || strchr(p, '.')) {
                // double
                char numbuf[64]; int ni2 = 0;
                while (((*p == '+' || *p == '-' || isdigit((unsigned char)*p) || *p == '.') && ni2 < (int)sizeof(numbuf)-1)) {
                    numbuf[ni2++] = *p++;
                }
                numbuf[ni2] = 0;
                double val = atof(numbuf);
                fputc(OP_PUSH_DOUBLE, fout);
                write_double(fout, val);
                fputc(OP_STORE, fout);
                write_name(fout, name);
            } else if (declared_type == VAL_BOOL || strncmp(p, "true", 4) == 0 || strncmp(p, "false", 5) == 0) {
                int bval = 0;
                if (strncmp(p, "true", 4) == 0) { bval = 1; p += 4; }
                else if (strncmp(p, "false", 5) == 0) { bval = 0; p += 5; }
                else { error_at(lineno, "Invalid bool literal"); }
                fputc(OP_PUSH_BOOL, fout);
                write_bool(fout, bval);
                fputc(OP_STORE, fout);
                write_name(fout, name);
            } else {
                // integer
                char numbuf[64]; int ni2 = 0;
                while ((*p == '+' || *p == '-' || isdigit((unsigned char)*p)) && ni2 < (int)sizeof(numbuf)-1) {
                    numbuf[ni2++] = *p++;
                }
                numbuf[ni2] = 0;
                if (ni2 == 0) { error_at(lineno, "Expected literal after ="); }
                int32_t val = atoi(numbuf);
                fputc(OP_PUSH_INT, fout);
                write_i32(fout, val);
                fputc(OP_STORE, fout);
                write_name(fout, name);
            }
            continue;
        }

        // handle cithak: <literal|identifier>
        if ((strncmp(p, "cithak", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))) {
            p += 6;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '"' || *p == '\'') {
                char quote = *p++;
                char buf[512]; int bl = 0;
                while (*p && *p != quote && bl < (int)sizeof(buf)-1) {
                    if (*p == '\\' && *(p+1)) {
                        p++;
                        if (*p == 'n') buf[bl++] = '\n';
                        else if (*p == 't') buf[bl++] = '\t';
                        else if (*p == '\\') buf[bl++] = '\\';
                        else buf[bl++] = *p;
                        p++;
                    } else {
                        buf[bl++] = *p++;
                    }
                }
                buf[bl] = 0;
                fputc(OP_PUSH_STR, fout);
                write_u16(fout, (uint16_t)bl);
                fwrite(buf, 1, bl, fout);
                fputc(OP_PRINT, fout);
            } else if (isalpha((unsigned char)*p) || *p == '_') {
                char name[128]; int ni = 0;
                while (is_identifier_char((unsigned char)*p) && ni < (int)sizeof(name)-1) name[ni++] = *p++;
                name[ni] = 0;
                fputc(OP_LOAD, fout);
                write_name(fout, name);
                fputc(OP_PRINT, fout);
            } else if (strchr(p, '+') || strchr(p, '-') || strchr(p, '*') || strchr(p, '/') || strchr(p, '%')) {
                emit_expression(fout, &p);
                fputc(OP_PRINT, fout);
            } else {
                // try integer literal
                char numbuf[64]; int ni2 = 0;
                while ((*p == '+' || *p == '-' || isdigit((unsigned char)*p)) && ni2 < (int)sizeof(numbuf)-1) {
                    numbuf[ni2++] = *p++;
                }
                if (ni2 == 0) { error_at(lineno, "Unknown print operand"); }
                numbuf[ni2] = 0;
                int32_t val = atoi(numbuf);
                fputc(OP_PUSH_INT, fout);
                write_i32(fout, val);
                fputc(OP_PRINT, fout);
            }
            continue;
        }

        // handle yen condition { ... }
        if ((strncmp(p, "yen", 3) == 0 && isspace((unsigned char)p[3]))) {
            p += 3;
            while (*p == ' ' || *p == '\t') p++;
            
            // Find opening brace
            char *brace = strchr(p, '{');
            if (!brace) { error_at(lineno, "Expected '{' after yen condition"); }
            
            // Extract and compile condition
            *brace = 0; // temporarily null-terminate condition
            char *condp = p;
            emit_expression(fout, &condp);
            *brace = '{'; // restore
            
            // Emit conditional jump (will be patched later)
            fputc(OP_JUMP_IF_FALSE, fout);
            long if_false_patch_pos = ftell(fout);
            write_i32(fout, 0); // placeholder for else start
            
            // Parse and compile statements inside if { ... }
            p = brace + 1;
            int brace_count = 1;
            while (fgets(line, sizeof(line), fin) && brace_count > 0) {
                lineno++;
                char *nested_p = line;
                while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                
                if (*nested_p == '}') {
                    brace_count--;
                    if (brace_count == 0) break;
                }
                if (*nested_p == '{') brace_count++;
                
                // Handle nested cithak statements
                if (strncmp(nested_p, "cithak", 6) == 0 && isspace((unsigned char)nested_p[6])) {
                    nested_p += 6;
                    while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                    emit_cithak_statement(fout, &nested_p);
                }
            }
            
            // Check if next line is "liyane"
            long after_if_pos = ftell(fout);
            long skip_else_patch_pos = 0;
            
            if (fgets(line, sizeof(line), fin)) {
                lineno++;
                char *next_p = line;
                while (*next_p == ' ' || *next_p == '\t') next_p++;
                
                if (strncmp(next_p, "liyane", 6) == 0) {
                    // Emit jump to skip else block (for true condition)
                    fseek(fout, after_if_pos, SEEK_SET);
                    fputc(OP_JUMP, fout);
                    skip_else_patch_pos = ftell(fout);
                    write_i32(fout, 0); // placeholder for end position
                    
                    // Now we're at the else start position
                    long else_start_pos = ftell(fout);
                    
                    // Patch the if-false jump to point here
                    fseek(fout, if_false_patch_pos, SEEK_SET);
                    write_i32(fout, (int32_t)else_start_pos);
                    fseek(fout, else_start_pos, SEEK_SET);
                    
                    // Parse else block
                    char *liyane_brace = strchr(next_p, '{');
                    if (liyane_brace) {
                        brace_count = 1;
                        while (fgets(line, sizeof(line), fin) && brace_count > 0) {
                            lineno++;
                            char *else_p = line;
                            while (*else_p == ' ' || *else_p == '\t') else_p++;
                            
                            if (*else_p == '}') {
                                brace_count--;
                                if (brace_count == 0) break;
                            }
                            if (*else_p == '{') brace_count++;
                            
                            // Handle nested cithak in else
                            if (strncmp(else_p, "cithak", 6) == 0 && isspace((unsigned char)else_p[6])) {
                                else_p += 6;
                                while (*else_p == ' ' || *else_p == '\t') else_p++;
                                emit_cithak_statement(fout, &else_p);
                            }
                        }
                    }
                    
                    // Patch the skip-else jump to point here (end of if-else)
                    long end_pos = ftell(fout);
                    fseek(fout, skip_else_patch_pos, SEEK_SET);
                    write_i32(fout, (int32_t)end_pos);
                    fseek(fout, end_pos, SEEK_SET);
                } else {
                    // No else, just patch the if-false jump to current position
                    fseek(fout, if_false_patch_pos, SEEK_SET);
                    write_i32(fout, (int32_t)after_if_pos);
                    fseek(fout, after_if_pos, SEEK_SET);
                    
                    // Reprocess this line since it's not "liyane"
                    fseek(fin, ftell(fin) - strlen(line), SEEK_SET);
                    lineno--;
                }
            } else {
                // EOF, patch if-false jump to current position
                fseek(fout, if_false_patch_pos, SEEK_SET);
                write_i32(fout, (int32_t)after_if_pos);
                fseek(fout, after_if_pos, SEEK_SET);
            }
            
            continue;
        }

        // handle menawa condition { ... } (while loop)
        if ((strncmp(p, "menawa", 6) == 0 && isspace((unsigned char)p[6]))) {
            p += 6;
            while (*p == ' ' || *p == '\t') p++;
            
            // Find opening parenthesis and brace
            char *paren = strchr(p, '(');
            if (!paren) { error_at(lineno, "Expected '(' after menawa"); }
            char *brace = strchr(paren, '{');
            if (!brace) { error_at(lineno, "Expected '{' after menawa condition"); }
            
            // Mark loop start position for back-jump
            long loop_start_pos = ftell(fout);
            
            // Extract and compile condition
            *brace = 0; // temporarily null-terminate condition
            char *condp = paren + 1;
            emit_expression(fout, &condp);
            *brace = '{'; // restore
            
            // Emit conditional jump to exit loop
            fputc(OP_JUMP_IF_FALSE, fout);
            long exit_patch_pos = ftell(fout);
            write_i32(fout, 0); // placeholder for loop exit
            
            // Parse and compile loop body
            p = brace + 1;
            int brace_count = 1;
            while (fgets(line, sizeof(line), fin) && brace_count > 0) {
                lineno++;
                char *nested_p = line;
                while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                
                if (*nested_p == '}') {
                    brace_count--;
                    if (brace_count == 0) break;
                }
                if (*nested_p == '{') brace_count++;
                
                // Handle nested cithak statements
                if (strncmp(nested_p, "cithak", 6) == 0 && isspace((unsigned char)nested_p[6])) {
                    nested_p += 6;
                    while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                    emit_cithak_statement(fout, &nested_p);
                }
                // Handle assignment statements in loop body
                else {
                    char *eq = strchr(nested_p, '=');
                    if (eq && nested_p[0] != '\0' && nested_p[0] != '\n') {
                        // Extract variable name
                        char name[128]; int ni = 0;
                        char *name_start = nested_p;
                        while (name_start < eq && isspace((unsigned char)*name_start)) name_start++;
                        while (name_start < eq && ni < sizeof(name)-1) {
                            if (!isspace((unsigned char)*name_start)) name[ni++] = *name_start;
                            name_start++;
                        }
                        name[ni] = 0;
                        
                        // Extract and compile expression after =
                        char *expr_start = eq + 1;
                        while (*expr_start == ' ' || *expr_start == '\t') expr_start++;
                        if (*expr_start && strlen(name) > 0) {
                            emit_expression(fout, &expr_start);
                            fputc(OP_STORE, fout);
                            write_name(fout, name);
                        }
                    }
                }
            }
            
            // Jump back to loop start (condition check)
            fputc(OP_JUMP, fout);
            write_i32(fout, (int32_t)loop_start_pos);
            
            // Patch exit jump to current position
            long loop_end_pos = ftell(fout);
            fseek(fout, exit_patch_pos, SEEK_SET);
            write_i32(fout, (int32_t)loop_end_pos);
            fseek(fout, loop_end_pos, SEEK_SET);
            
            continue;
        }

        // handle ngulang (init; condition; increment) { ... } (for loop)
        if ((strncmp(p, "ngulang", 7) == 0 && isspace((unsigned char)p[7]))) {
            p += 7;
            while (*p == ' ' || *p == '\t') p++;
            
            // Find opening parenthesis and brace
            char *paren = strchr(p, '(');
            if (!paren) { error_at(lineno, "Expected '(' after ngulang"); }
            char *brace = strchr(paren, '{');
            if (!brace) { error_at(lineno, "Expected '{' after ngulang condition"); }
            
            // Parse for loop components: (init; condition; increment)
            *brace = 0; // temporarily null-terminate
            char *for_content = paren + 1;
            
            // Find semicolons to separate init, condition, increment
            char *semi1 = strchr(for_content, ';');
            char *semi2 = semi1 ? strchr(semi1 + 1, ';') : NULL;
            if (!semi1 || !semi2) {
                error_at(lineno, "Expected format: ngulang (init; condition; increment)");
            }
            
            // Parse init statement
            *semi1 = 0;
            if (strlen(for_content) > 0) {
                char *init_p = for_content;
                while (*init_p == ' ' || *init_p == '\t') init_p++;
                if (*init_p) {
                    // Simple assignment: variable = value
                    char *eq = strchr(init_p, '=');
                    if (eq) {
                        char name[128]; int ni = 0;
                        while (init_p < eq && ni < sizeof(name)-1) {
                            if (*init_p != ' ' && *init_p != '\t') name[ni++] = *init_p;
                            init_p++;
                        }
                        name[ni] = 0;
                        init_p = eq + 1;
                        while (*init_p == ' ' || *init_p == '\t') init_p++;
                        emit_expression(fout, &init_p);
                        fputc(OP_STORE, fout);
                        write_name(fout, name);
                    }
                }
            }
            
            // Mark loop start for condition check
            long loop_start_pos = ftell(fout);
            
            // Parse condition
            char *cond_start = semi1 + 1;
            *semi2 = 0;
            while (*cond_start == ' ' || *cond_start == '\t') cond_start++;
            if (strlen(cond_start) > 0) {
                emit_expression(fout, &cond_start);
                fputc(OP_JUMP_IF_FALSE, fout);
            } else {
                // No condition = infinite loop, push true
                fputc(OP_PUSH_BOOL, fout);
                fputc(1, fout);
                fputc(OP_JUMP_IF_FALSE, fout);
            }
            long exit_patch_pos = ftell(fout);
            write_i32(fout, 0); // placeholder
            
            *brace = '{'; // restore brace
            
            // Parse loop body
            p = brace + 1;
            int brace_count = 1;
            while (fgets(line, sizeof(line), fin) && brace_count > 0) {
                lineno++;
                char *nested_p = line;
                while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                
                if (*nested_p == '}') {
                    brace_count--;
                    if (brace_count == 0) break;
                }
                if (*nested_p == '{') brace_count++;
                
                // Handle nested cithak statements
                if (strncmp(nested_p, "cithak", 6) == 0 && isspace((unsigned char)nested_p[6])) {
                    nested_p += 6;
                    while (*nested_p == ' ' || *nested_p == '\t') nested_p++;
                    emit_cithak_statement(fout, &nested_p);
                }
                // Handle assignment statements in for loop body
                else {
                    char *eq = strchr(nested_p, '=');
                    if (eq && nested_p[0] != '\0' && nested_p[0] != '\n') {
                        // Extract variable name
                        char name[128]; int ni = 0;
                        char *name_start = nested_p;
                        while (name_start < eq && isspace((unsigned char)*name_start)) name_start++;
                        while (name_start < eq && ni < sizeof(name)-1) {
                            if (!isspace((unsigned char)*name_start)) name[ni++] = *name_start;
                            name_start++;
                        }
                        name[ni] = 0;
                        
                        // Extract and compile expression after =
                        char *expr_start = eq + 1;
                        while (*expr_start == ' ' || *expr_start == '\t') expr_start++;
                        if (*expr_start && strlen(name) > 0) {
                            emit_expression(fout, &expr_start);
                            fputc(OP_STORE, fout);
                            write_name(fout, name);
                        }
                    }
                }
            }
            
            // Execute increment statement
            char *incr_start = semi2 + 1;
            while (*incr_start == ' ' || *incr_start == '\t') incr_start++;
            char *paren_end = strchr(incr_start, ')');
            if (paren_end) *paren_end = 0;
            
            if (strlen(incr_start) > 0) {
                char *eq = strchr(incr_start, '=');
                if (eq) {
                    char name[128]; int ni = 0;
                    while (incr_start < eq && ni < sizeof(name)-1) {
                        if (*incr_start != ' ' && *incr_start != '\t') name[ni++] = *incr_start;
                        incr_start++;
                    }
                    name[ni] = 0;
                    incr_start = eq + 1;
                    while (*incr_start == ' ' || *incr_start == '\t') incr_start++;
                    emit_expression(fout, &incr_start);
                    fputc(OP_STORE, fout);
                    write_name(fout, name);
                }
            }
            
            // Jump back to condition check
            fputc(OP_JUMP, fout);
            write_i32(fout, (int32_t)loop_start_pos);
            
            // Patch exit jump
            long loop_end_pos = ftell(fout);
            fseek(fout, exit_patch_pos, SEEK_SET);
            write_i32(fout, (int32_t)loop_end_pos);
            fseek(fout, loop_end_pos, SEEK_SET);
            
            continue;
        }

        // Handle assignment statements (var = expr)
        char *eq = strchr(p, '=');
        if (eq && !strchr(p, '"') && !strchr(p, '\'')) { // avoid string literals
            // Extract variable name
            char name[128]; int ni = 0;
            char *name_start = p;
            while (name_start < eq && isspace((unsigned char)*name_start)) name_start++;
            while (name_start < eq && ni < sizeof(name)-1) {
                if (!isspace((unsigned char)*name_start)) name[ni++] = *name_start;
                name_start++;
            }
            name[ni] = 0;
            
            // Extract and compile expression after =
            char *expr_start = eq + 1;
            while (*expr_start == ' ' || *expr_start == '\t') expr_start++;
            if (*expr_start && strlen(name) > 0) {
                emit_expression(fout, &expr_start);
                fputc(OP_STORE, fout);
                write_name(fout, name);
                continue;
            }
        }

    // unknown statement
    error_at(lineno, "Unknown or unsupported statement");
    }

    fputc(OP_HALT, fout);

    fclose(fin);
    fclose(fout);
    printf("Compiled %s → %s\n", src, out);
}
