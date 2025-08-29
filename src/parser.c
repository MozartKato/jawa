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
    // Always promote to double for arithmetic to prevent integer overflow
    if (a == TY_INT && b == TY_INT) {
        return TY_DOUBLE;  // Force double for int arithmetic to prevent overflow
    }
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
        // Check for 'this' keyword in OOP context
        if (strcmp(t.lex, "this") == 0) {
            lex_next(L); // consume 'this'
            
            // Must be followed by dot notation
            if (L->cur.kind == T_DOT) {
                lex_next(L); // consume '.'
                
                if (L->cur.kind == T_IDENT) {
                    char property_name[64];
                    strncpy(property_name, L->cur.lex, sizeof(property_name) - 1);
                    property_name[sizeof(property_name) - 1] = '\0';
                    lex_next(L);
                    
                    // Generate this->property_name access
                    sb_add(out, outsz, oi, "this->%s", property_name);
                    
                    // Return appropriate type (for now default to string, we'll improve this)
                    return TY_STRING;
                } else {
                    // Error: expected property name after 'this.'
                    sb_add(out, outsz, oi, "this");
                    return TY_UNK;
                }
            } else {
                // Error: 'this' must be followed by '.'
                sb_add(out, outsz, oi, "this");
                return TY_UNK;
            }
        }
        
        // Check for built-in string functions and method calls
        if (strcmp(t.lex, "cithak") == 0 ||
            strcmp(t.lex, "toUpperCase") == 0 || strcmp(t.lex, "toLowerCase") == 0 ||
            strcmp(t.lex, "menyang_gedhe") == 0 || strcmp(t.lex, "menyang_cilik") == 0 ||
            strcmp(t.lex, "split") == 0 || strcmp(t.lex, "pisah") == 0 ||
            strcmp(t.lex, "concat") == 0 || strcmp(t.lex, "gabung") == 0 ||
            strcmp(t.lex, "replace") == 0 || strcmp(t.lex, "ganti") == 0 ||
            strcmp(t.lex, "substring") == 0 || strcmp(t.lex, "potong") == 0 ||
            strcmp(t.lex, "length") == 0 || strcmp(t.lex, "dawane") == 0) {
            
            char func_name[64];
            strncpy(func_name, t.lex, sizeof(func_name) - 1);
            func_name[sizeof(func_name) - 1] = '\0';
            lex_next(L);
            
            if (L->cur.kind == T_LPAREN) {
                lex_next(L); // consume '('
                
                // Special handling for int_to_string (no prefix needed)
                if (strcmp(func_name, "int_to_string") == 0) {
                    sb_add(out, outsz, oi, "int_to_string(");
                } else {
                    // Map to corresponding C function with string_ prefix
                    sb_add(out, outsz, oi, "string_%s(", func_name);
                }
                
                // Parse arguments
                int arg_count = 0;
                while (L->cur.kind != T_RPAREN && L->cur.kind != T_EOF) {
                    if (arg_count > 0) {
                        sb_add(out, outsz, oi, ", ");
                    }
                    parse_expr(L, out, outsz, oi, ctx);
                    arg_count++;
                    
                    if (L->cur.kind == T_COMMA) {
                        lex_next(L);
                    }
                }
                
                if (L->cur.kind == T_RPAREN) {
                    lex_next(L); // consume ')'
                }
                
                sb_add(out, outsz, oi, ")");
                return TY_STRING; // Most string functions return string
            }
        }
        
        // Regular variable access
        char var_name[64];
        strncpy(var_name, t.lex, sizeof(var_name) - 1);
        var_name[sizeof(var_name) - 1] = '\0';
        Ty ty = parser_get_var_type(ctx, t.lex);
        lex_next(L);
        
        // Check if this is a function call
        if (L->cur.kind == T_LPAREN) {
            // This is a function call
            lex_next(L); // consume '('
            
            // Add function name
            sb_add(out, outsz, oi, "%s(", var_name);
            
            // Parse arguments
            int arg_count = 0;
            while (L->cur.kind != T_RPAREN && L->cur.kind != T_EOF) {
                if (arg_count > 0) {
                    sb_add(out, outsz, oi, ", ");
                }
                parse_expr(L, out, outsz, oi, ctx);
                arg_count++;
                
                if (L->cur.kind == T_COMMA) {
                    lex_next(L);
                }
            }
            
            if (L->cur.kind == T_RPAREN) {
                lex_next(L); // consume ')'
            }
            
            sb_add(out, outsz, oi, ")");
            return ty == TY_UNK ? TY_DOUBLE : ty; // Default to double if type unknown
        }
        
        // Check for method chaining with dot notation
        if (L->cur.kind == T_DOT) {
            lex_next(L); // consume '.'
            
            if (L->cur.kind == T_IDENT) {
                char method_name[64];
                strncpy(method_name, L->cur.lex, sizeof(method_name) - 1);
                method_name[sizeof(method_name) - 1] = '\0';
                lex_next(L);
                
                // Check if it's a method call
                if (L->cur.kind == T_LPAREN) {
                    lex_next(L); // consume '('
                    
                    // Special handling for split method
                    if (strcmp(method_name, "split") == 0 || strcmp(method_name, "pisah") == 0) {
                        // Generate unique variable for this split operation
                        static int split_counter = 0;
                        char split_var[32];
                        snprintf(split_var, sizeof(split_var), "split_count_%d", split_counter++);
                        
                        sb_add(out, outsz, oi, "({ int %s = 0; char** split_result = string_split(%s", split_var, var_name);
                        if (L->cur.kind != T_RPAREN) {
                            sb_add(out, outsz, oi, ", ");
                            parse_expr(L, out, outsz, oi, ctx);
                            sb_add(out, outsz, oi, ", &%s); create_array_literal_from_split(split_result, %s); })", split_var, split_var);
                        } else {
                            sb_add(out, outsz, oi, ", \",\", &%s); create_array_literal_from_split(split_result, %s); })", split_var, split_var);
                        }
                        
                        if (L->cur.kind == T_RPAREN) {
                            lex_next(L);
                        }
                        return TY_ARRAY;
                    } else if (strcmp(method_name, "length") == 0 || strcmp(method_name, "dawane") == 0) {
                        // Special handling for length() method call - return as string for concatenation
                        if (ty == TY_STRING) {
                            sb_add(out, outsz, oi, "int_to_string(string_length(%s))", var_name);
                        } else if (ty == TY_ARRAY) {
                            sb_add(out, outsz, oi, "int_to_string(array_get_length(%s))", var_name);
                        }
                        
                        if (L->cur.kind == T_RPAREN) {
                            lex_next(L);
                        }
                        return TY_STRING; // Return as string for easy concatenation
                    } else {
                        // Generate method call with object as first parameter  
                        // Map Javanese method names to English equivalents
                        const char* actual_method = method_name;
                        if (strcmp(method_name, "menyang_gedhe") == 0) actual_method = "toUpperCase";
                        else if (strcmp(method_name, "menyang_cilik") == 0) actual_method = "toLowerCase";
                        else if (strcmp(method_name, "pisah") == 0) actual_method = "split";
                        else if (strcmp(method_name, "gabung") == 0) actual_method = "concat";
                        else if (strcmp(method_name, "ganti") == 0) actual_method = "replace";
                        else if (strcmp(method_name, "potong") == 0) actual_method = "substring";
                        else if (strcmp(method_name, "dawane") == 0) actual_method = "length";
                        
                        sb_add(out, outsz, oi, "string_%s(%s", actual_method, var_name);
                        
                        // Parse method arguments
                        if (L->cur.kind != T_RPAREN) {
                            sb_add(out, outsz, oi, ", ");
                            while (L->cur.kind != T_RPAREN && L->cur.kind != T_EOF) {
                                parse_expr(L, out, outsz, oi, ctx);
                                if (L->cur.kind == T_COMMA) {
                                    lex_next(L);
                                    sb_add(out, outsz, oi, ", ");
                                }
                            }
                        }
                        
                        if (L->cur.kind == T_RPAREN) {
                            lex_next(L); // consume ')'
                        }
                        
                        sb_add(out, outsz, oi, ")");
                        return TY_STRING;
                    }
                } else {
                    // Property access (like .length or .dawane)
                    if (strcmp(method_name, "length") == 0 || strcmp(method_name, "dawane") == 0) {
                        if (ty == TY_STRING) {
                            sb_add(out, outsz, oi, "string_length(%s)", var_name);
                            return TY_INT;
                        } else if (ty == TY_ARRAY) {
                            sb_add(out, outsz, oi, "array_get_length(%s)", var_name);
                            return TY_INT;
                        }
                    }
                    // Default to variable access
                    sb_add(out, outsz, oi, "%s", var_name);
                    return ty;
                }
            }
        } else {
            sb_add(out, outsz, oi, "%s", var_name);
        }
        
        // Check for array indexing
        while (L->cur.kind == T_LBRACKET) {
            lex_next(L); // consume '['
            
            // Generate array_get call, first get current expression
            char current_expr[2048];  // Increased buffer size
            strncpy(current_expr, out, *oi);
            current_expr[*oi] = 0;
            
            // Reset output and generate array_get call
            *oi = 0;
            sb_add(out, outsz, oi, "array_get_string(");
            sb_add(out, outsz, oi, "%s", current_expr);
            sb_add(out, outsz, oi, ", ");
            parse_expr(L, out, outsz, oi, ctx);
            
            if (L->cur.kind == T_RBRACKET) {
                lex_next(L); // consume ']'
            }
            
            sb_add(out, outsz, oi, ")");
            ty = TY_STRING; // Array elements are strings
        }
        
        return ty ? ty : TY_UNK;
    }
    if (t.kind == T_LPAREN) { 
        lex_next(L); 
        Ty inner = parse_expr(L, out, outsz, oi, ctx); 
        if (L->cur.kind == T_RPAREN) lex_next(L); 
        return inner; 
    }
    if (t.kind == T_LBRACKET) {
        // Array literal parsing
        lex_next(L); // consume '['
        
        sb_add(out, outsz, oi, "create_array_literal(");
        int element_count = 0;
        
        while (L->cur.kind != T_RBRACKET && L->cur.kind != T_EOF) {
            if (element_count > 0) {
                sb_add(out, outsz, oi, ", ");
            }
            parse_expr(L, out, outsz, oi, ctx);
            element_count++;
            
            if (L->cur.kind == T_COMMA) {
                lex_next(L);
            }
        }
        
        if (L->cur.kind == T_RBRACKET) {
            lex_next(L); // consume ']'
        }
        
        sb_add(out, outsz, oi, ", %d)", element_count);
        return TY_ARRAY;
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
        
        if (op == T_PLUS && left == TY_STRING) {
            // String concatenation - capture left expression  
            char left_expr[4096];  // Increased buffer size
            int left_len = *oi;
            strncpy(left_expr, out, left_len);
            left_expr[left_len] = 0;
            
            lex_next(L); // consume '+'
            
            // Parse right operand
            char right_expr[4096];  // Increased buffer size
            int right_oi = 0;
            Ty right = parse_mul(L, right_expr, sizeof(right_expr), &right_oi, ctx);
            right_expr[right_oi] = 0;
            
            // Auto-convert types for string concatenation
            char final_right[4096];  // Increased buffer size
            
            // Prevent buffer overflow by checking input length
            size_t right_len = strlen(right_expr);
            if (right_len > 3900) {  // Leave room for wrapper functions
                fprintf(stderr, "Warning: Expression too long, truncating\n");
                strncpy(final_right, right_expr, sizeof(final_right) - 1);
                final_right[sizeof(final_right) - 1] = '\0';
            } else if (right == TY_INT) {
                snprintf(final_right, sizeof(final_right), "int_to_string(%s)", right_expr);
            } else if (right == TY_DOUBLE) {
                snprintf(final_right, sizeof(final_right), "double_to_string(%s)", right_expr);
            } else if (right == TY_BOOL) {
                snprintf(final_right, sizeof(final_right), "(%s) ? \"true\" : \"false\"", right_expr);
            } else {
                strncpy(final_right, right_expr, sizeof(final_right) - 1);
                final_right[sizeof(final_right) - 1] = '\0';  // Ensure null termination
            }
            
            // Generate string_concat call
            *oi = 0;
            sb_add(out, outsz, oi, "string_concat(%s, %s)", left_expr, final_right);
            left = TY_STRING;
        } else {
            // Numeric operations
            lex_next(L);
            sb_add(out, outsz, oi, " %s ", op == T_PLUS ? "+" : "-");
            Ty right = parse_mul(L, out, outsz, oi, ctx);
            left = promote_num(left, right, 0);
        }
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
