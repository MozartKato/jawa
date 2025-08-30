#include "statements.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>  // Add for strtoll

static int g_if_depth = 0;

bool is_variable_declaration(const char *line) {
    return (strncmp(line, "owahi ", 6) == 0 || 
            strncmp(line, "cendhak ", 8) == 0 || 
            strncmp(line, "ajek ", 5) == 0);
}

bool is_print_statement(const char *line) {
    return (strncmp(line, "cithak", 6) == 0 && (isspace((unsigned char)line[6]) || line[6] == '('));
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
    return strncmp(line, "ngulang", 7) == 0 && (isspace((unsigned char)line[7]) || line[7] == '(');
}

bool is_function_definition(const char *line) {
    return strncmp(line, "gawe ", 5) == 0;
}

bool is_return_statement(const char *line) {
    return strncmp(line, "bali", 4) == 0;
}

bool is_standalone_function_call(const char *line) {
    // Look for pattern: identifier followed by opening parenthesis
    // but not variable declaration or assignment
    if (is_variable_declaration(line) || is_assignment(line) || is_print_statement(line)) {
        return false;
    }
    
    // Find opening parenthesis
    char *paren = strchr(line, '(');
    if (!paren) return false;
    
    // Check if there's a valid identifier before the parenthesis
    const char *p = line;
    while (*p && isspace((unsigned char)*p)) p++;
    
    if (!isalpha((unsigned char)*p) && *p != '_') return false;
    
    // Check identifier is valid
    while (p < paren && (isalnum((unsigned char)*p) || *p == '_')) p++;
    while (p < paren && isspace((unsigned char)*p)) p++;
    
    return (p == paren);  // Should be exactly at the opening parenthesis
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
        else if (strcmp(ctype, "array") == 0) { mapped = "JawaArray*"; vty = TY_ARRAY; }
    } else {
        // Type inference when no explicit type - parse expression first
        char ebuf[4096]; 
        int oi = 0; 
        Lexer L = {.p = q}; 
        lex_next(&L); 
        Ty expr_type = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
        
        if (expr_type == TY_STRING) { 
            mapped = "const char*"; 
            vty = TY_STRING; 
        } else if (expr_type == TY_ARRAY) { 
            mapped = "JawaArray*"; 
            vty = TY_ARRAY; 
        } else if (expr_type == TY_BOOL) { 
            mapped = "bool"; 
            vty = TY_BOOL; 
        } else if (expr_type == TY_INT) { 
            // Check if the integer literal is too large for int type
            long long num_value = strtoll(q, NULL, 10);
            if (num_value > 2147483647LL || num_value < -2147483648LL) {
                // Integer literal too large, use double instead
                mapped = "double"; 
                vty = TY_DOUBLE;
            } else {
                mapped = "int"; 
                vty = TY_INT; 
            }
        } else {
            // Fallback - inspect the literal value
            if (*q == '"') {
                mapped = "const char*"; 
                vty = TY_STRING;
            } else if (strstr(q, "true") == q || strstr(q, "false") == q) {
                mapped = "bool"; 
                vty = TY_BOOL;
            } else if (strchr(q, '.')) {
                mapped = "double"; 
                vty = TY_DOUBLE;
            } else if (isdigit(*q)) {
                // Smart number detection - check if number is too large for int
                long long num_value = strtoll(q, NULL, 10);
                // Debug: fprintf(stderr, "DEBUG: Number %lld, max int: %d\n", num_value, 2147483647);
                if (num_value > 2147483647LL || num_value < -2147483648LL) {
                    // Number too large for int, use double for large integers
                    mapped = "double"; 
                    vty = TY_DOUBLE;
                    // Debug: fprintf(stderr, "DEBUG: Using double for large number\n");
                } else {
                    mapped = "int"; 
                    vty = TY_INT;
                    // Debug: fprintf(stderr, "DEBUG: Using int for small number\n");
                }
            }
        }
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
    
    // Check if using parentheses syntax: cithak("text")
    bool has_parens = (*q == '(');
    if (has_parens) {
        q++; // skip opening parenthesis
        // Find matching closing parenthesis
        const char *closing_paren = strrchr(q, ')');
        if (closing_paren) {
            // Extract content between parentheses
            int content_len = closing_paren - q;
            char content[1024];
            strncpy(content, q, content_len);
            content[content_len] = 0;
            
            // Parse the content as expression
            char ebuf[4096];
            int oi = 0;
            ebuf[0] = 0;
            Lexer L = {.p = content};
            lex_next(&L);
            Ty et = parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
            
            if (et == TY_STRING) {
                fprintf(out, "printf(\"%%s\\n\", %s);\n", ebuf);
            } else if (et == TY_BOOL) {
                fprintf(out, "printf(\"%%s\\n\", (%s) ? \"true\" : \"false\");\n", ebuf);
            } else {
                fprintf(out, "printf(\"%%g\\n\", (double)(%s));\n", ebuf);
            }
            return;
        }
    }
    
    // Original parsing for space-separated syntax: cithak "text"
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
                    
                    // Skip type name (support both English and Javanese types)
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
                    (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);  // Result not needed, cast to void
                    
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
            // Check for i++ pattern
            if (strstr(incr_start, "++")) {
                char var_name[128];
                int ni = 0;
                while (is_ident_char((unsigned char)*incr_start) && ni < 127) {
                    var_name[ni++] = *incr_start++;
                }
                var_name[ni] = 0;
                fprintf(out, "%s++", var_name);
            } else {
                // Handle assignment format like i = i + 1
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
            // Check if this is a 'this.property' assignment
            char processed_var[128];
            if (strncmp(name_start, "this.", 5) == 0) {
                // Convert this.property to this->property
                snprintf(processed_var, sizeof(processed_var), "this->%s", name_start + 5);
            } else {
                strcpy(processed_var, name_start);
            }
            
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
                
                fprintf(out, "%s = %s;\n", processed_var, ebuf);
            }
        }
    }
}

void parse_function_definition(const char *line, FILE *out, ParserContext *ctx) {
    // Skip "gawe" and whitespace
    const char *p = line + 4;
    while (*p && isspace((unsigned char)*p)) p++;
    
    // Parse function name
    char func_name[128] = {0};
    int ni = 0;
    
    while (is_ident_char((unsigned char)*p) && ni < 127) {
        func_name[ni++] = *p++;
    }
    func_name[ni] = 0;
    
    // Skip to parameters
    while (*p && isspace((unsigned char)*p)) p++;
    
    // Check for opening parenthesis
    if (*p != '(') {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        return;
    }
    p++;
    
    // Get parameters
    char param_list[512] = {0};
    int pi = 0;
    
    int paren_depth = 1;
    while (*p && paren_depth > 0) {
        if (*p == '(') paren_depth++;
        else if (*p == ')') paren_depth--;
        
        if (paren_depth > 0) {
            param_list[pi++] = *p++;
        } else {
            p++; // Skip closing parenthesis
        }
    }
    param_list[pi] = 0;
    
    // Skip to return type if specified
    while (*p && isspace((unsigned char)*p)) p++;
    
    // Check for return type
    char return_type[32] = "double"; // Default return type
    if (*p == ':') {
        p++;
        while (*p && isspace((unsigned char)*p)) p++;
        
        int ti = 0;
        while (is_ident_char((unsigned char)*p) && ti < 31) {
            return_type[ti++] = *p++;
        }
        return_type[ti] = 0;
    }
    
    // Map Jawa types to C types
    const char *c_return_type = "double";
    if (strcmp(return_type, "int") == 0) c_return_type = "int";
    else if (strcmp(return_type, "double") == 0) c_return_type = "double";
    else if (strcmp(return_type, "bool") == 0) c_return_type = "bool";
    else if (strcmp(return_type, "string") == 0) c_return_type = "const char*";
    
    // Process parameters for C type declaration
    char c_param_list[1024] = {0};
    
    if (strlen(param_list) > 0) {
        // Split parameters by commas and add types
        char *param_list_copy = strdup(param_list);
        char *token = strtok(param_list_copy, ",");
        int first = 1;
        
        while (token) {
            // Trim whitespace
            char *param = token;
            while (*param && isspace((unsigned char)*param)) param++;
            char *end = param + strlen(param) - 1;
            while (end > param && isspace((unsigned char)*end)) *end-- = 0;
            
            if (strlen(param) > 0) {
                if (!first) {
                    strcat(c_param_list, ", ");
                }
                
                // Parse parameter with type annotation: "name: type" or just "name"
                char param_name[128] = {0};
                char param_type[32] = "double"; // default
                
                char *colon = strchr(param, ':');
                if (colon) {
                    // Has type annotation
                    int name_len = colon - param;
                    if (name_len > 0 && name_len < 127) {
                        strncpy(param_name, param, name_len);
                        param_name[name_len] = 0;
                        
                        // Trim spaces from name
                        char *name_end = param_name + strlen(param_name) - 1;
                        while (name_end > param_name && isspace((unsigned char)*name_end)) *name_end-- = 0;
                        
                        // Extract type
                        char *type_start = colon + 1;
                        while (*type_start && isspace((unsigned char)*type_start)) type_start++;
                        
                        if (strlen(type_start) > 0) {
                            strncpy(param_type, type_start, 31);
                            param_type[31] = 0;
                            
                            // Trim spaces from type
                            char *type_end = param_type + strlen(param_type) - 1;
                            while (type_end > param_type && isspace((unsigned char)*type_end)) *type_end-- = 0;
                        }
                    }
                } else {
                    // No type annotation, just name
                    strncpy(param_name, param, 127);
                    param_name[127] = 0;
                }
                
                // Map Jawa type to C type
                const char *c_param_type = "double";
                if (strcmp(param_type, "int") == 0) c_param_type = "int";
                else if (strcmp(param_type, "double") == 0) c_param_type = "double";
                else if (strcmp(param_type, "bool") == 0) c_param_type = "bool";
                else if (strcmp(param_type, "string") == 0) c_param_type = "const char*";
                
                strcat(c_param_list, c_param_type);
                strcat(c_param_list, " ");
                strcat(c_param_list, param_name);
                first = 0;
                
                // Register parameter as a variable in the parser context
                Ty param_ty = TY_DOUBLE;
                if (strcmp(param_type, "int") == 0) param_ty = TY_INT;
                else if (strcmp(param_type, "string") == 0) param_ty = TY_STRING;
                else if (strcmp(param_type, "bool") == 0) param_ty = TY_BOOL;
                
                parser_add_var(ctx, param_name, param_ty);
            }
            
            token = strtok(NULL, ",");
        }
        
        free(param_list_copy);
    }
    
    // Generate function definition
    fprintf(out, "%s %s(%s) {\n", c_return_type, func_name, c_param_list);
    
    // Function is complete when we see the closing brace
    // The function body will be processed separately
}

void parse_return_statement(const char *line, FILE *out, ParserContext *ctx) {
    // Skip "bali" and whitespace
    const char *p = line + 4;
    while (*p && isspace((unsigned char)*p)) p++;
    
    // Parse the return expression
    char ebuf[4096];
    int oi = 0;
    ebuf[0] = 0;
    
    Lexer L = {.p = p};
    lex_next(&L);
    (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
    
    fprintf(out, "return %s;\n", ebuf);
}

void parse_standalone_function_call(const char *line, FILE *out, ParserContext *ctx) {
    // Parse the entire line as a function call expression
    char ebuf[4096];
    int oi = 0;
    ebuf[0] = 0;
    
    Lexer L = {.p = line};
    lex_next(&L);
    (void)parse_expr(&L, ebuf, sizeof(ebuf), &oi, ctx);
    
    fprintf(out, "%s;\n", ebuf);
}
