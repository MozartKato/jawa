#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

#include "lexer.h"
#include "parser.h"
#include "statements.h"
#include "file_ops.h"
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
    fputs("char* long_to_string(long long value);\n", c);
    fputs("char* double_to_string(double value);\n\n", c);
    
    // Add file operations function prototypes
    fputs("// Built-in file operations functions\n", c);
    fputs("typedef struct JawaFile JawaFile;\n", c);
    fputs("typedef struct FileInfo FileInfo;\n", c);
    fputs("JawaFile* file_buka(const char *path, const char *mode);\n", c);
    fputs("void file_tutup(JawaFile *jf);\n", c);
    fputs("char* file_waca_kabeh(JawaFile *jf);\n", c);
    fputs("char* file_waca_baris(JawaFile *jf);\n", c);
    fputs("int file_tulis(JawaFile *jf, const char *text);\n", c);
    fputs("int file_tulis_baris(JawaFile *jf, const char *text);\n", c);
    fputs("int file_akhir(JawaFile *jf);\n", c);
    fputs("FileInfo* file_info(const char *path);\n", c);
    fputs("bool file_ada(const char *path);\n", c);
    fputs("bool file_hapus(const char *path);\n", c);
    fputs("bool file_ubah_nama(const char *old_path, const char *new_path);\n", c);
    fputs("bool file_ubah_posisi(JawaFile *jf, long offset, int whence);\n", c);
    fputs("long file_posisi(JawaFile *jf);\n", c);
    fputs("char* file_waca_bytes(JawaFile *jf, size_t bytes);\n", c);
    fputs("const char* file_error_message(JawaFile *jf);\n", c);
    fputs("bool file_bisa_dibaca(const char *path);\n", c);
    fputs("bool file_bisa_ditulis(const char *path);\n", c);
    fputs("bool file_buat_direktori(const char *path);\n", c);
    fputs("bool file_adalah_direktori(const char *path);\n", c);
    fputs("char** file_list_direktori(const char *path, int *count);\n\n", c);
    
    // Add system command function prototype  
    fputs("// Built-in system functions\n", c);
    fputs("int system(const char *command);\n\n", c);
    
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
    
    // Method parsing state
    char current_method_name[64] = {0};
    char current_method_params[256] = {0};
    char current_method_return_type[32] = {0};
    
    rewind(in);
    
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line); 
        if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        
        const char *p = line; 
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;

        // Handle bolo (class) declaration
        if (strncmp(p, "bolo ", 5) == 0) {
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
                // Parse method name and parameters
                const char *q = p + 5; // Skip "gawe "
                while (*q && isspace((unsigned char)*q)) q++;
                
                // Extract method name
                char method_name[64] = {0};
                int ni = 0;
                while (is_ident_char((unsigned char)*q) && ni < 63) {
                    method_name[ni++] = *q++;
                }
                
                // Skip whitespace and find parameters
                while (*q && isspace((unsigned char)*q)) q++;
                
                // Extract parameters (between parentheses)
                char method_params[256] = {0};
                char method_return_type[32] = "void";
                
                if (*q == '(') {
                    q++; // skip '('
                    int pi = 0;
                    int paren_depth = 1;
                    while (*q && paren_depth > 0 && pi < 255) {
                        if (*q == '(') paren_depth++;
                        else if (*q == ')') paren_depth--;
                        
                        if (paren_depth > 0) {
                            method_params[pi++] = *q;
                        }
                        q++;
                    }
                    
                    // Look for return type after )
                    while (*q && isspace((unsigned char)*q)) q++;
                    if (*q == ':') {
                        q++;
                        while (*q && isspace((unsigned char)*q)) q++;
                        
                        // Extract return type
                        int ri = 0;
                        while (is_ident_char((unsigned char)*q) && ri < 31) {
                            method_return_type[ri++] = *q++;
                        }
                        method_return_type[ri] = 0;
                        
                        // Convert Jawa types to C types
                        if (strcmp(method_return_type, "string") == 0) {
                            strcpy(method_return_type, "const char*");
                        }
                    }
                }
                
                // Store method info for later when we finish parsing body
                strncpy(current_method_name, method_name, sizeof(current_method_name) - 1);
                current_method_name[sizeof(current_method_name) - 1] = 0;
                
                strncpy(current_method_params, method_params, sizeof(current_method_params) - 1);
                current_method_params[sizeof(current_method_params) - 1] = 0;
                
                strncpy(current_method_return_type, method_return_type, sizeof(current_method_return_type) - 1);
                current_method_return_type[sizeof(current_method_return_type) - 1] = 0;
                
                in_method = true;
                method_body_len = 0;
                method_body[0] = 0;
                continue;
            }
            
            // Collect method body
            if (in_method) {
                if (brace_depth == 0 && strchr(p, '}')) {
                    // End of method - process collected body
                    // For now, use simple method body with proper statement parsing later
                    // The method_body contains raw statements that need processing
                    
                    class_add_method(current_class, current_method_name, current_method_return_type, 
                                   current_method_params, method_body, false);
                    in_method = false;
                    
                    // Clear method info
                    current_method_name[0] = 0;
                    current_method_params[0] = 0;
                    current_method_return_type[0] = 0;
                } else if (strlen(p) > 0) {
                    // Skip empty lines and opening braces
                    const char *trimmed = p;
                    while (*trimmed && isspace((unsigned char)*trimmed)) trimmed++;
                    
                    if (strlen(trimmed) > 0 && strcmp(trimmed, "{") != 0) {
                        if (method_body_len + strlen(trimmed) + 10 < sizeof(method_body)) {
                            if (method_body_len > 0) {
                                strcat(method_body, "\n");
                                method_body_len++;
                            }
                            
                            // Store raw line - will be processed by statement parser
                            strcat(method_body, trimmed);
                            method_body_len += strlen(trimmed);
                        }
                    }
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

static void parse_function_definitions(FILE *in, FILE *out, ParserContext *ctx) {
    char line[512];
    bool in_function = false;
    bool in_function_body = false;
    bool in_class = false;
    int class_brace_depth = 0;
    
    rewind(in);
    
    // First pass: Generate function prototypes
    fputs("// Function prototypes\n", out);
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line); 
        if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        
        const char *p = line; 
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;
        
        // Track class sections to skip methods inside classes
        if (strncmp(p, "bolo ", 5) == 0) {
            in_class = true;
            class_brace_depth = 0;
            continue;
        }
        
        if (in_class) {
            // Count braces in class
            for (const char *c = p; *c; c++) {
                if (*c == '{') class_brace_depth++;
                if (*c == '}') class_brace_depth--;
            }
            
            if (class_brace_depth <= 0) {
                in_class = false;
            }
            continue; // Skip everything inside class
        }

        if (strncmp(p, "gawe ", 5) == 0) {
            // Extract function name for prototype
            const char *name_start = p + 5;
            while (*name_start && isspace((unsigned char)*name_start)) name_start++;
            
            char func_name[64];
            int i = 0;
            while (*name_start && *name_start != '(' && !isspace((unsigned char)*name_start) && i < 63) {
                func_name[i++] = *name_start++;
            }
            func_name[i] = 0;
            
            // Parse return type if specified  
            const char *return_part = strchr(p, ')');
            const char *c_return_type = "double"; // default
            if (return_part) {
                return_part++; // Skip ')'
                while (*return_part && isspace((unsigned char)*return_part)) return_part++;
                if (*return_part == ':') {
                    return_part++;
                    while (*return_part && isspace((unsigned char)*return_part)) return_part++;
                    
                    if (strncmp(return_part, "string", 6) == 0) c_return_type = "const char*";
                    else if (strncmp(return_part, "int", 3) == 0) c_return_type = "int";
                    else if (strncmp(return_part, "bool", 4) == 0) c_return_type = "bool";
                    else if (strncmp(return_part, "double", 6) == 0) c_return_type = "double";
                }
            }
            
            // Default return type is double, default params are typed based on annotation
            fprintf(out, "%s %s(", c_return_type, func_name);
            
            // Find parameter list
            const char *paren_start = strchr(p, '(');
            const char *paren_end = strchr(p, ')');
            if (paren_start && paren_end && paren_end > paren_start) {
                const char *param_str = paren_start + 1;
                int param_len = paren_end - param_str;
                char param_list[256];
                if (param_len > 0 && param_len < 255) {
                    strncpy(param_list, param_str, param_len);
                    param_list[param_len] = 0;
                    
                    // Trim whitespace
                    char *start = param_list;
                    while (*start && isspace((unsigned char)*start)) start++;
                    char *end = param_list + strlen(param_list) - 1;
                    while (end > start && isspace((unsigned char)*end)) *end-- = 0;
                    
                    if (strlen(start) > 0) {
                        // Split by comma and generate typed params
                        char *param_copy = strdup(start);
                        char *token = strtok(param_copy, ",");
                        bool first = true;
                        while (token) {
                            // Trim token
                            while (*token && isspace((unsigned char)*token)) token++;
                            char *token_end = token + strlen(token) - 1;
                            while (token_end > token && isspace((unsigned char)*token_end)) *token_end-- = 0;
                            
                            if (!first) fprintf(out, ", ");
                            
                            // Parse parameter with type annotation: "name: type" or just "name"
                            char *colon = strchr(token, ':');
                            if (colon) {
                                // Has type annotation
                                int name_len = colon - token;
                                char param_name[128];
                                if (name_len > 0 && name_len < 127) {
                                    strncpy(param_name, token, name_len);
                                    param_name[name_len] = 0;
                                    
                                    // Trim spaces from name
                                    char *name_end = param_name + strlen(param_name) - 1;
                                    while (name_end > param_name && isspace((unsigned char)*name_end)) *name_end-- = 0;
                                    
                                    // Extract type
                                    char *type_start = colon + 1;
                                    while (*type_start && isspace((unsigned char)*type_start)) type_start++;
                                    
                                    // Map type
                                    const char *c_type = "double";
                                    if (strncmp(type_start, "int", 3) == 0) c_type = "int";
                                    else if (strncmp(type_start, "string", 6) == 0) c_type = "const char*";
                                    else if (strncmp(type_start, "bool", 4) == 0) c_type = "bool";
                                    
                                    fprintf(out, "%s %s", c_type, param_name);
                                }
                            } else {
                                // No type annotation, default to double
                                fprintf(out, "double %s", token);
                            }
                            first = false;
                            
                            token = strtok(NULL, ",");
                        }
                        free(param_copy);
                    }
                }
            }
            fprintf(out, ");\n");
        }
    }
    fputs("\n", out);
    
    // Second pass: Generate function implementations
    rewind(in);
    fputs("// Function implementations\n", out);
    in_class = false;
    class_brace_depth = 0;
    
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line); 
        if (len && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]=0;
        
        const char *p = line; 
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) continue;
        
        // Track class sections to skip methods inside classes
        if (strncmp(p, "bolo ", 5) == 0) {
            in_class = true;
            class_brace_depth = 0;
            continue;
        }
        
        if (in_class) {
            // Count braces in class
            for (const char *c = p; *c; c++) {
                if (*c == '{') class_brace_depth++;
                if (*c == '}') class_brace_depth--;
            }
            
            if (class_brace_depth <= 0) {
                in_class = false;
            }
            continue; // Skip everything inside class
        }

        if (strncmp(p, "gawe ", 5) == 0) {
            in_function = true;
            parse_function_definition(p, out, ctx);
            continue;
        }
        
        if (in_function) {
            if (strcmp(p, "{") == 0) {
                in_function_body = true;
                // Don't write extra {, function header already has it
                continue;
            } else if (strcmp(p, "}") == 0) {
                in_function_body = false;
                in_function = false;
                fputs("}\n\n", out);
            } else if (in_function_body) {
                // Parse function body statements
                if (is_variable_declaration(p)) {
                    parse_variable_declaration(p, out, ctx);
                } else if (is_print_statement(p)) {
                    parse_print_statement(p, out, ctx);
                } else if (is_return_statement(p)) {
                    parse_return_statement(p, out, ctx);
                } else if (is_assignment(p)) {
                    parse_assignment(p, out, ctx);
                } else if (is_standalone_function_call(p)) {
                    parse_standalone_function_call(p, out, ctx);
                }
            }
        }
    }
}
static void parse_main_program(FILE *in, FILE *out, ParserContext *ctx) {
    char line[512];
    bool in_function = false;
    
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

        // Skip function definitions (already handled in parse_function_definitions)
        if (strncmp(p, "gawe ", 5) == 0) {
            in_function = true;
            continue;
        }
        
        // Skip function body content (already handled)
        if (in_function) {
            if (strcmp(p, "}") == 0) {
                in_function = false;
            }
            continue;
        }

        // Parse main program statements only
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
        } else if (is_standalone_function_call(p)) {
            parse_standalone_function_call(p, out, ctx);
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
    
    // Parse function definitions
    ParserContext ctx;
    parser_init(&ctx);
    parse_function_definitions(in, c, &ctx);
    
    // Parse main program
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
    
    // Add print function implementations
    fputs("// Print function implementations\n", c);
    fputs("void tulis(const char* text) {\n", c);
    fputs("    if (text) printf(\"%s\\n\", text);\n", c);
    fputs("}\n\n", c);
    fputs("void cithak(const char* text) {\n", c);
    fputs("    if (text) printf(\"%s\\n\", text);\n", c);
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
    
    // Add int_to_string implementation - Enhanced for large numbers
    fputs("char* int_to_string(int value) {\n", c);
    fputs("    char* result = malloc(32);\n", c);
    fputs("    sprintf(result, \"%d\", value);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add long_to_string implementation for large integers
    fputs("char* long_to_string(long long value) {\n", c);
    fputs("    char* result = malloc(32);\n", c);
    fputs("    sprintf(result, \"%lld\", value);\n", c);
    fputs("    return result;\n", c);
    fputs("}\n\n", c);
    
    // Add double_to_string implementation - Enhanced  
    fputs("char* double_to_string(double value) {\n", c);
    fputs("    char* result = malloc(32);\n", c);
    fputs("    // Check if it's a whole number larger than int range\n", c);
    fputs("    if (value == (long long)value && (value > 2147483647.0 || value < -2147483648.0)) {\n", c);
    fputs("        sprintf(result, \"%.0f\", value);  // Print as integer without decimal\n", c);
    fputs("    } else if (value == (long long)value) {\n", c);
    fputs("        sprintf(result, \"%.0f\", value);  // Print small integers without decimal\n", c); 
    fputs("    } else {\n", c);
    fputs("        sprintf(result, \"%.2f\", value);  // Print decimals normally\n", c);
    fputs("    }\n", c);
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
    snprintf(cmd, sizeof(cmd), "cc -O2 -std=c11 -o '%s' '%s' obj/file_ops.o", outPath, tmpc);
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
