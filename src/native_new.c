#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "lexer.h"
#include "parser.h"
#include "statements.h"
#include "oop/class.h"

static void write_preamble(FILE *c) {
    fputs("#include <stdio.h>\n#include <stdint.h>\n#include <stdbool.h>\n\n", c);
}

static void skip_class_block(FILE *in, char *line, size_t line_size) {
    int class_brace_depth = 0;
    do {
        for (const char *c = line; *c; c++) {
            if (*c == '{') class_brace_depth++;
            if (*c == '}') class_brace_depth--;
        }
        if (class_brace_depth <= 0) break;
    } while (fgets(line, line_size, in));
}

static void parse_classes(FILE *in, FILE *out) {
    char line[512];
    JawaClass *current_class = NULL;
    bool in_class = false;
    bool in_method = false;
    char method_body[2048] = {0};
    int method_body_len = 0;
    int brace_depth = 0;
    
    rewind(in);
    
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line); 
        if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        
        const char *p = line; 
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;

        // Handle bolo (class) declaration
        if (strncmp(p, "bolo ", 5) == 0 && isspace((unsigned char)p[5])) {
            const char *q = p + 5; 
            while (*q && isspace((unsigned char)*q)) q++;
            
            char class_name[64]; 
            int ni = 0;
            while (is_ident_char((unsigned char)*q) && ni < 63) {
                class_name[ni++] = *q++;
            }
            class_name[ni] = 0;
            
            current_class = class_create(class_name);
            in_class = true;
            brace_depth = 0;
            continue;
        }
        
        if (in_class && current_class) {
            // Count braces
            for (const char *c = p; *c; c++) {
                if (*c == '{') brace_depth++;
                if (*c == '}') brace_depth--;
            }
            
            // Handle property declarations inside class
            if (is_variable_declaration(p)) {
                // Parse property: owahi name: type = value
                const char *q = p + (strncmp(p, "owahi", 5) == 0 ? 6 : 
                                    strncmp(p, "cendhak", 7) == 0 ? 8 : 5);
                while (*q && isspace((unsigned char)*q)) q++;
                
                char prop_name[64]; 
                int ni = 0;
                while (is_ident_char((unsigned char)*q) && ni < 63) {
                    prop_name[ni++] = *q++;
                }
                prop_name[ni] = 0;
                
                // Skip to type
                while (*q && (*q != ':')) q++;
                if (*q == ':') q++;
                while (*q && isspace((unsigned char)*q)) q++;
                
                char prop_type[32]; 
                ni = 0;
                while (is_ident_char((unsigned char)*q) && ni < 31) {
                    prop_type[ni++] = *q++;
                }
                prop_type[ni] = 0;
                
                PropertyType ptype = PROP_STRING;
                if (strcmp(prop_type, "int") == 0) ptype = PROP_INT;
                else if (strcmp(prop_type, "double") == 0) ptype = PROP_DOUBLE;
                else if (strcmp(prop_type, "bool") == 0) ptype = PROP_BOOL;
                
                bool is_private = (strncmp(line, "    ", 4) == 0);
                class_add_property(current_class, prop_name, ptype, is_private);
                continue;
            }
            
            // Handle gawe (method) inside class
            if (strncmp(p, "gawe ", 5) == 0) {
                in_method = true;
                method_body_len = 0;
                method_body[0] = 0;
                continue;
            }
            
            // Collect method body
            if (in_method) {
                if (method_body_len + strlen(p) + 2 < sizeof(method_body)) {
                    if (method_body_len > 0) {
                        strcat(method_body, "\n");
                        method_body_len++;
                    }
                    strcat(method_body, "    ");
                    strcat(method_body, p);
                    method_body_len += strlen(p) + 4;
                }
                
                if (brace_depth == 0 && strchr(p, '}')) {
                    // End of method
                    class_add_method(current_class, "", "void", "", method_body, false);
                    in_method = false;
                }
            }
            
            // End of class
            if (brace_depth == 0 && strchr(p, '}')) {
                in_class = false;
                current_class = NULL;
            }
        }
    }
    
    // Generate all class code
    generate_object_c_code(out);
}

static void parse_main_program(FILE *in, FILE *out, ParserContext *ctx) {
    char line[512];
    
    rewind(in);
    fputs("int main(){\n", out);
    
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line); 
        if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        
        const char *p = line; 
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;

        // Skip class declarations and their content
        if (strncmp(p, "bolo ", 5) == 0 && isspace((unsigned char)p[5])) {
            skip_class_block(in, line, sizeof(line));
            continue;
        }

        // Skip standalone function declarations
        if (strncmp(p, "gawe ", 5) == 0 && isspace((unsigned char)p[5])) {
            continue;
        }

        // Parse different statement types
        if (is_variable_declaration(p)) {
            parse_variable_declaration(p, out, ctx);
        } else if (is_print_statement(p)) {
            parse_print_statement(p, out, ctx);
        } else if (is_if_statement(p)) {
            parse_if_statement(p, out, ctx);
        } else if (is_else_statement(p)) {
            parse_else_statement(p, out);
        } else if (is_while_statement(p)) {
            parse_while_statement(p, out, ctx);
        } else if (is_for_statement(p)) {
            parse_for_statement(p, out, ctx);
        } else if (strcmp(p, "{") == 0) {
            fputs("{\n", out);
        } else if (strcmp(p, "}") == 0) {
            fputs("}\n", out);
        } else if (is_assignment(p)) {
            parse_assignment(p, out, ctx);
        }
        // Other statements can be added here
    }

    fputs("return 0;\n}\n", out);
}

int build_native(const char *srcPath, const char *outPath) {
    FILE *in = fopen(srcPath, "r");
    if (!in) { 
        perror("open src"); 
        return 1; 
    }
    
    char tmpc[FILENAME_MAX];
    snprintf(tmpc, sizeof(tmpc), "%s.c", outPath);
    FILE *c = fopen(tmpc, "w");
    if (!c) { 
        perror("open c out"); 
        fclose(in); 
        return 2; 
    }

    write_preamble(c);
    
    // Parse classes first
    parse_classes(in, c);
    
    // Parse main program
    ParserContext ctx;
    parser_init(&ctx);
    parse_main_program(in, c, &ctx);
    
    fclose(in); 
    fclose(c);

    // Compile C code
    char cmd[8192];
    snprintf(cmd, sizeof(cmd), "cc -O2 -std=c11 -o '%s' '%s'", outPath, tmpc);
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "Failed to build native binary (rc=%d)\n", rc);
        unlink(tmpc);
        return 3;
    }
    
    // Clean up temporary C file
    unlink(tmpc);
    return 0;
}
