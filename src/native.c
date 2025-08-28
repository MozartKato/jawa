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
    fputs("#include <stdio.h>\n#include <stdint.h>\n#include <stdbool.h>\n", c);
    fputs("#include <string.h>\n#include <stdlib.h>\n#include <ctype.h>\n\n", c);
    
    // Add string helper function
    fputs("// String helper functions\n", c);
    fputs("char* str_dup(const char* s) {\n", c);
    fputs("    if (!s) return NULL;\n", c);
    fputs("    int len = strlen(s);\n", c);
    fputs("    char* result = malloc(len + 1);\n", c);
    fputs("    strcpy(result, s);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add global variable for split operations  
    fputs("int temp_count = 0;\n", c);
    fputs("int debug_split_count = 0;\n\n", c);
    
    // Add string function prototypes
    fputs("// Built-in string functions\n", c);
    fputs("char* string_toUpperCase(const char* str);\n", c);
    fputs("char* string_toLowerCase(const char* str);\n", c);
    fputs("int string_length(const char* str);\n", c);
    fputs("char* string_substring(const char* str, int start, int end);\n", c);
    fputs("char* string_replace(const char* str, const char* old_str, const char* new_str);\n", c);
    fputs("int string_indexOf(const char* str, const char* search);\n", c);
    fputs("char* string_concat(const char* a, const char* b);\n", c);
    fputs("char** string_split(const char* str, const char* delimiter, int* count);\n", c);
    fputs("char* int_to_string(int value);\n", c);
    fputs("char* double_to_string(double value);\n\n", c);
    
    // Add array function prototypes
    fputs("// Built-in array functions\n", c);
    fputs("typedef struct { char** data; int size; int capacity; } JawaArray;\n", c);
    fputs("JawaArray* create_array_literal(...);\n", c);
    fputs("JawaArray* create_array_literal_from_split(char** elements, int count);\n", c);
    fputs("char* array_get_string(JawaArray* arr, int index);\n", c);
    fputs("int array_get_length(JawaArray* arr);\n\n", c);
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
    
    // Add string function implementations
    fputs("\n// String function implementations\n", c);
    fputs("char* string_toUpperCase(const char* str) {\n", c);
    fputs("    if (!str) return NULL;\n", c);
    fputs("    int len = strlen(str);\n", c);
    fputs("    char* result = malloc(len + 1);\n", c);
    fputs("    for (int i = 0; i < len; i++) {\n", c);
    fputs("        result[i] = toupper(str[i]);\n", c);
    fputs("    }\n", c);
    fputs("    result[len] = 0;\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    fputs("char* string_toLowerCase(const char* str) {\n", c);
    fputs("    if (!str) return NULL;\n", c);
    fputs("    int len = strlen(str);\n", c);
    fputs("    char* result = malloc(len + 1);\n", c);
    fputs("    for (int i = 0; i < len; i++) {\n", c);
    fputs("        result[i] = tolower(str[i]);\n", c);
    fputs("    }\n", c);
    fputs("    result[len] = 0;\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    fputs("int string_length(const char* str) {\n", c);
    fputs("    return str ? strlen(str) : 0;\n", c);
    fputs("}\n\n", c);
    
    fputs("char* string_substring(const char* str, int start, int end) {\n", c);
    fputs("    if (!str) return NULL;\n", c);
    fputs("    int len = strlen(str);\n", c);
    fputs("    if (start < 0) start = 0;\n", c);
    fputs("    if (end > len) end = len;\n", c);
    fputs("    if (start >= end) return str_dup(\"\");\n", c);
    fputs("    int result_len = end - start;\n", c);
    fputs("    char* result = malloc(result_len + 1);\n", c);
    fputs("    strncpy(result, str + start, result_len);\n", c);
    fputs("    result[result_len] = 0;\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    fputs("char* string_replace(const char* str, const char* old_str, const char* new_str) {\n", c);
    fputs("    if (!str || !old_str || !new_str) return str_dup(str ? str : \"\");\n", c);
    fputs("    char* pos = strstr(str, old_str);\n", c);
    fputs("    if (!pos) return str_dup(str);\n", c);
    fputs("    int old_len = strlen(old_str);\n", c);
    fputs("    int new_len = strlen(new_str);\n", c);
    fputs("    int str_len = strlen(str);\n", c);
    fputs("    char* result = malloc(str_len - old_len + new_len + 1);\n", c);
    fputs("    int prefix_len = pos - str;\n", c);
    fputs("    strncpy(result, str, prefix_len);\n", c);
    fputs("    strcpy(result + prefix_len, new_str);\n", c);
    fputs("    strcpy(result + prefix_len + new_len, pos + old_len);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    fputs("char* string_concat(const char* a, const char* b) {\n", c);
    fputs("    if (!a) a = \"\";\n", c);
    fputs("    if (!b) b = \"\";\n", c);
    fputs("    int len_a = strlen(a);\n", c);
    fputs("    int len_b = strlen(b);\n", c);
    fputs("    char* result = malloc(len_a + len_b + 1);\n", c);
    fputs("    strcpy(result, a);\n", c);
    fputs("    strcat(result, b);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add int_to_string implementation
    fputs("char* int_to_string(int value) {\n", c);
    fputs("    char* result = malloc(32);\n", c);
    fputs("    sprintf(result, \"%d\", value);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add double_to_string implementation  
    fputs("char* double_to_string(double value) {\n", c);
    fputs("    char* result = malloc(32);\n", c);
    fputs("    sprintf(result, \"%.2f\", value);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add string_split implementation
    fputs("char** string_split(const char* str, const char* delimiter, int* count) {\n", c);
    fputs("    if (!str || !delimiter) { *count = 0; return NULL; }\n", c);
    fputs("    char* str_copy = str_dup(str);\n", c);
    fputs("    char** result = malloc(100 * sizeof(char*));\n", c);
    fputs("    *count = 0;\n", c);
    fputs("    char* token = strtok(str_copy, delimiter);\n", c);
    fputs("    while (token != NULL && *count < 100) {\n", c);
    fputs("        result[*count] = str_dup(token);\n", c);
    fputs("        (*count)++;\n", c);
    fputs("        token = strtok(NULL, delimiter);\n", c);
    fputs("    }\n", c);
    fputs("    free(str_copy);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add array functions
    fputs("JawaArray* create_array_literal_from_split(char** elements, int count) {\n", c);
    fputs("    if (!elements || count <= 0) {\n", c);
    fputs("        JawaArray* empty = malloc(sizeof(JawaArray));\n", c);
    fputs("        empty->data = NULL;\n", c);
    fputs("        empty->size = 0;\n", c);
    fputs("        empty->capacity = 0;\n", c);
    fputs("        return empty;\n", c);
    fputs("    }\n", c);
    fputs("    JawaArray* arr = malloc(sizeof(JawaArray));\n", c);
    fputs("    arr->data = elements;\n", c);
    fputs("    arr->size = count;\n", c);
    fputs("    arr->capacity = count;\n", c);
    fputs("    return arr;\n", c);
    fputs("}\n\n", c);
    
    fputs("char* array_get_string(JawaArray* arr, int index) {\n", c);
    fputs("    if (!arr || index < 0 || index >= arr->size) return \"\";\n", c);
    fputs("    return arr->data[index];\n", c);
    fputs("}\n\n", c);
    
    fputs("int array_get_length(JawaArray* arr) {\n", c);
    fputs("    return arr ? arr->size : 0;\n", c);
    fputs("}\n\n", c);
    
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
