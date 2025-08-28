#include "statements.h"
#include <string.h>
#include <ctype.h>

static int g_if_depth = 0;

bool is_variable_declaration(const char *line) {
    return (strncmp(line, "owahi ", 6) == 0 || 
            strncmp(line, "cendhak ", 8) == 0 || 
            strncmp(line, "ajek ", 5) == 0);
}

bool is_print_statement(const char *line) {
    return strncmp(line, "cithak", 6) == 0 && isspace((unsigned char)line[6]);
}

bool is_if_statement(const char *line) {
    return strncmp(line, "yen", 3) == 0 && isspace((unsigned char)line[3]);
}

bool is_else_statement(const char *line) {
    return strstr(line, "} liyane {") != NULL || strncmp(line, "liyane", 6) == 0;
}

bool is_while_statement(const char *line) {
    return strncmp(line, "menawa", 6) == 0 && isspace((unsigned char)line[6]);
}

bool is_for_statement(const char *line) {
    return strncmp(line, "ngulang", 7) == 0 && isspace((unsigned char)line[7]);
}

bool is_assignment(const char *line) {
    char *eq_pos = strchr(line, '=');
    if (!eq_pos) return false;
    
    // Check it's not a declaration
    if (is_variable_declaration(line)) return false;
    
    // Check there's a valid identifier before =
    char var_name[128];
    int name_len = (int)(eq_pos - line);
    if (name_len > 0 && name_len < 127) {
        strncpy(var_name, line, name_len);
        var_name[name_len] = 0;
        
        char *name_start = var_name;
        while (*name_start && isspace((unsigned char)*name_start)) name_start++;
        
        return strlen(name_start) > 0 && is_ident_char((unsigned char)*name_start);
    }
    return false;
}

void parse_variable_declaration(const char *line, FILE *out, ParserContext *ctx) {
    const char *q = line;
    if (strncmp(line, "owahi ", 6) == 0) q = line + 6;
    else if (strncmp(line, "cendhak ", 8) == 0) q = line + 8;
    else if (strncmp(line, "ajek ", 5) == 0) q = line + 5;
    
    while (*q == ' ') q++;
    
    // Parse variable name
    char name[128]; 
    int ni = 0;
    while (is_ident_char((unsigned char)*q) && ni < 127) {
        name[ni++] = *q++;
    }
    name[ni] = 0;
    
    while (*q == ' ') q++;
    
    // Parse type annotation
    char ctype[32] = {0};
    if (*q == ':') { 
        q++; 
        while (*q == ' ') q++; 
        int ti = 0; 
        while (is_ident_char((unsigned char)*q) && ti < 31) {
            ctype[ti++] = *q++;
        }
        ctype[ti] = 0; 
        while (*q == ' ') q++; 
    }
    
    if (*q == '=') q++;
    while (*q == ' ') q++;
    
    // Map types to C types
    const char *mapped = "double";
    Ty vty = TY_DOUBLE;
    if (ctype[0]) {
        if (strcmp(ctype, "int") == 0) { mapped = "int"; vty = TY_INT; }
        else if (strcmp(ctype, "double") == 0) { mapped = "double"; vty = TY_DOUBLE; }
        else if (strcmp(ctype, "bool") == 0) { mapped = "bool"; vty = TY_BOOL; }
        else if (strcmp(ctype, "string") == 0) { mapped = "const char*"; vty = TY_STRING; }
    }
    
    parser_add_var(ctx, name, vty);
    fprintf(out, "%s %s = ", mapped, name);
    
    char ebuf[4096]; 
    int oi = 0; 
    ebuf[0] = 0;
    Lexer L = { .p = q };
    lex_next(&L);
    (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
    fputs(ebuf, out);
    fputs(";\n", out);
}

void parse_print_statement(const char *line, FILE *out, ParserContext *ctx) {
    const char *q = line + 6; 
    while (*q && isspace((unsigned char)*q)) q++;
    
    // Parse multiple arguments separated by comma
    char remaining[1024];
    strncpy(remaining, q, sizeof(remaining) - 1);
    remaining[sizeof(remaining) - 1] = 0;
    
    char *arg = remaining;
    while (arg && *arg) {
        // Find next comma or end
        char *next_arg = strchr(arg, ',');
        if (next_arg) {
            *next_arg = 0;
            next_arg++;
            while (*next_arg && isspace((unsigned char)*next_arg)) next_arg++;
        }
        
        // Trim current arg
        while (*arg && isspace((unsigned char)*arg)) arg++;
        char *arg_end = arg + strlen(arg) - 1;
        while (arg_end > arg && isspace((unsigned char)*arg_end)) *arg_end-- = 0;
        
        if (strlen(arg) > 0) {
            if (*arg == '"') {
                char ebuf[4096]; 
                int oi = 0; 
                ebuf[0] = 0; 
                Lexer L = {.p = arg}; 
                lex_next(&L); 
                Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx); 
                (void)et;
                fprintf(out, "printf(\"%%s \", %s);\n", ebuf);
            } else {
                char ebuf[4096]; 
                int oi = 0; 
                ebuf[0] = 0; 
                Lexer L = {.p = arg}; 
                lex_next(&L); 
                Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
                
                if (et == TY_STRING) {
                    fprintf(out, "printf(\"%%s \", %s);\n", ebuf);
                } else if (et == TY_BOOL) {
                    fprintf(out, "printf(\"%%s \", (%s) ? \"true\" : \"false\");\n", ebuf);
                } else {
                    fprintf(out, "printf(\"%%g \", (double)(%s));\n", ebuf);
                }
            }
        }
        arg = next_arg;
    }
    fprintf(out, "printf(\"\\n\");\n");
}

void parse_if_statement(const char *line, FILE *out, ParserContext *ctx) {
    const char *q = line + 3; 
    while (*q && isspace((unsigned char)*q)) q++;
    
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
            char ebuf[4096]; 
            int oi = 0; 
            ebuf[0] = 0; 
            Lexer L = {.p = cond_buf}; 
            lex_next(&L); 
            Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
            
            fprintf(out, "if (");
            if (et == TY_BOOL) {
                fprintf(out, "%s", ebuf);
            } else {
                fprintf(out, "(%s)", ebuf); // treat non-zero as true
            }
            fputs(") {\n", out);
            g_if_depth++;
        }
    }
}

void parse_else_statement(const char *line, FILE *out) {
    if (strstr(line, "} liyane {") != NULL) {
        fputs("} else {\n", out);
    } else if (strncmp(line, "liyane", 6) == 0) {
        const char *q = line + 6;
        while (*q && isspace((unsigned char)*q)) q++;
        if (*q == '{' || *q == 0) {
            fputs("} else {\n", out);
        }
    }
}

void parse_while_statement(const char *line, FILE *out, ParserContext *ctx) {
    const char *q = line + 6; 
    while (*q && isspace((unsigned char)*q)) q++;
    
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
            char ebuf[4096]; 
            int oi = 0; 
            ebuf[0] = 0; 
            Lexer L = {.p = cond_buf}; 
            lex_next(&L); 
            Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
            
            fprintf(out, "while (");
            if (et == TY_BOOL) {
                fprintf(out, "%s", ebuf);
            } else {
                fprintf(out, "(%s)", ebuf); // treat non-zero as true
            }
            fputs(") {\n", out);
        }
    }
}

void parse_for_statement(const char *line, FILE *out, ParserContext *ctx) {
    const char *q = line + 7; 
    while (*q && isspace((unsigned char)*q)) q++;
    
    // Find the opening brace
    const char *brace_pos = strchr(q, '{');
    if (!brace_pos) return;
    
    // Extract for statement (everything before {)
    int for_len = (int)(brace_pos - q);
    char for_buf[512]; 
    if (for_len >= (int)sizeof(for_buf)) return;
    
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
    
    fputs("for (", out);
    
    if (semi1 && semi2) {
        // Init statement
        *semi1 = 0;
        if (strlen(start) > 0) {
            if (strncmp(start, "owahi", 5) == 0) {
                // Handle variable declaration like "owahi i: int = 1"
                const char *var_start = start + 5;
                while (*var_start && isspace((unsigned char)*var_start)) var_start++;
                
                char var_name[128]; 
                int ni = 0;
                while (is_ident_char((unsigned char)*var_start) && ni < 127) {
                    var_name[ni++] = *var_start++;
                }
                var_name[ni] = 0;
                
                while (*var_start && isspace((unsigned char)*var_start)) var_start++;
                
                // Skip type annotation if present
                if (*var_start == ':') {
                    var_start++;
                    while (*var_start && isspace((unsigned char)*var_start)) var_start++;
                    while (is_ident_char((unsigned char)*var_start)) var_start++;
                    while (*var_start && isspace((unsigned char)*var_start)) var_start++;
                }
                
                if (*var_start == '=') {
                    var_start++;
                    while (*var_start && isspace((unsigned char)*var_start)) var_start++;
                    
                    char ebuf[1024]; 
                    int oi = 0; 
                    ebuf[0] = 0;
                    Lexer L = {.p = var_start}; 
                    lex_next(&L);
                    Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
                    
                    if (et == TY_DOUBLE || et == TY_UNK) {
                        parser_add_var(ctx, var_name, TY_DOUBLE);
                        fprintf(out, "double %s = %s", var_name, ebuf);
                    } else {
                        parser_add_var(ctx, var_name, TY_INT);
                        fprintf(out, "int %s = %s", var_name, ebuf);
                    }
                }
            } else {
                // Handle regular assignment like "i = 1" - auto-declare as int
                char *eq = strchr(start, '=');
                if (eq) {
                    *eq = 0;
                    char *var_name = start;
                    while (*var_name && isspace((unsigned char)*var_name)) var_name++;
                    char *var_end = eq - 1;
                    while (var_end > var_name && isspace((unsigned char)*var_end)) *var_end-- = 0;
                    
                    char *val_start = eq + 1;
                    while (*val_start && isspace((unsigned char)*val_start)) val_start++;
                    
                    char ebuf[1024]; 
                    int oi = 0; 
                    ebuf[0] = 0;
                    Lexer L = {.p = val_start}; 
                    lex_next(&L);
                    Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
                    
                    // Auto-declare as int for loop variables
                    parser_add_var(ctx, var_name, TY_INT);
                    fprintf(out, "int %s = %s", var_name, ebuf);
                }
            }
        }
        
        fputs("; ", out);
        
        // Condition
        char *cond_start = semi1 + 1;
        *semi2 = 0;
        while (*cond_start && isspace((unsigned char)*cond_start)) cond_start++;
        if (strlen(cond_start) > 0) {
            char ebuf[1024]; 
            int oi = 0; 
            ebuf[0] = 0;
            Lexer L = {.p = cond_start}; 
            lex_next(&L);
            Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
            (void)et;
            fprintf(out, "%s", ebuf);
        }
        
        fputs("; ", out);
        
        // Increment
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
                
                char ebuf[1024]; 
                int oi = 0; 
                ebuf[0] = 0;
                Lexer L = {.p = val_start}; 
                lex_next(&L);
                Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
                (void)et;
                
                fprintf(out, "%s = %s", var_name, ebuf);
            }
        }
    }
    
    fputs(") {\n", out);
}

void parse_assignment(const char *line, FILE *out, ParserContext *ctx) {
    char *eq_pos = strchr(line, '=');
    if (!eq_pos) return;
    
    // Extract variable name
    char var_name[128];
    int name_len = (int)(eq_pos - line);
    if (name_len > 0 && name_len < 127) {
        strncpy(var_name, line, name_len);
        var_name[name_len] = 0;
        
        // Trim spaces from variable name
        char *name_start = var_name;
        while (*name_start && isspace((unsigned char)*name_start)) name_start++;
        char *name_end = var_name + strlen(var_name) - 1;
        while (name_end > name_start && isspace((unsigned char)*name_end)) *name_end-- = 0;
        
        if (strlen(name_start) > 0) {
            // Parse expression after =
            char *expr_start = eq_pos + 1;
            while (*expr_start && isspace((unsigned char)*expr_start)) expr_start++;
            
            if (strlen(expr_start) > 0) {
                char ebuf[1024];
                int oi = 0;
                ebuf[0] = 0;
                
                Lexer L = {.p = expr_start};
                lex_next(&L);
                (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
                
                fprintf(out, "%s = %s;\n", name_start, ebuf);
            }
        }
    }
}
