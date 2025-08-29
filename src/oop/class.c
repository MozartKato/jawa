#include "oop/class.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global class registry
JawaClass *g_classes = NULL;

JawaClass* class_create(const char *name) {
    JawaClass *cls = malloc(sizeof(JawaClass));
    if (!cls) return NULL;
    
    strncpy(cls->name, name, sizeof(cls->name) - 1);
    cls->name[sizeof(cls->name) - 1] = 0;
    cls->properties = NULL;
    cls->methods = NULL;
    cls->parent = NULL;
    cls->next = g_classes;
    g_classes = cls;
    
    return cls;
}

void class_add_property(JawaClass *cls, const char *name, PropertyType type, bool is_private) {
    if (!cls) return;
    
    JawaProperty *prop = malloc(sizeof(JawaProperty));
    if (!prop) return;
    
    strncpy(prop->name, name, sizeof(prop->name) - 1);
    prop->name[sizeof(prop->name) - 1] = 0;
    prop->type = type;
    prop->is_private = is_private;
    prop->next = cls->properties;
    cls->properties = prop;
}

void class_add_method(JawaClass *cls, const char *name, const char *return_type,
                     const char *params, const char *body, bool is_private) {
    if (!cls) return;
    
    JawaMethod *method = malloc(sizeof(JawaMethod));
    if (!method) return;
    
    strncpy(method->name, name, sizeof(method->name) - 1);
    method->name[sizeof(method->name) - 1] = 0;
    
    strncpy(method->return_type, return_type, sizeof(method->return_type) - 1);
    method->return_type[sizeof(method->return_type) - 1] = 0;
    
    strncpy(method->params, params, sizeof(method->params) - 1);
    method->params[sizeof(method->params) - 1] = 0;
    
    strncpy(method->body, body, sizeof(method->body) - 1);
    method->body[sizeof(method->body) - 1] = 0;
    
    method->is_private = is_private;
    method->next = cls->methods;
    cls->methods = method;
}

JawaClass* class_find(const char *name) {
    JawaClass *cls = g_classes;
    while (cls) {
        if (strcmp(cls->name, name) == 0) {
            return cls;
        }
        cls = cls->next;
    }
    return NULL;
}

void class_set_parent(JawaClass *cls, JawaClass *parent) {
    if (cls) {
        cls->parent = parent;
    }
}

JawaObject* object_create(JawaClass *cls) {
    if (!cls) return NULL;
    
    JawaObject *obj = malloc(sizeof(JawaObject));
    if (!obj) return NULL;
    
    obj->class_def = cls;
    obj->data = NULL; // TODO: Allocate property storage
    obj->next = NULL;
    
    return obj;
}

void object_destroy(JawaObject *obj) {
    if (obj) {
        if (obj->data) free(obj->data);
        free(obj);
    }
}

void generate_class_c_code(FILE *out, JawaClass *cls) {
    if (!out || !cls) return;
    
    // Generate struct definition
    fprintf(out, "// Class: %s\n", cls->name);
    fprintf(out, "typedef struct %s {\n", cls->name);
    
    // Generate properties
    JawaProperty *prop = cls->properties;
    while (prop) {
        const char *c_type = "void*";
        switch (prop->type) {
            case PROP_INT: c_type = "int"; break;
            case PROP_DOUBLE: c_type = "double"; break;
            case PROP_BOOL: c_type = "bool"; break;
            case PROP_STRING: c_type = "char*"; break;
            case PROP_OBJECT: c_type = "void*"; break;
        }
        fprintf(out, "    %s %s;\n", c_type, prop->name);
        prop = prop->next;
    }
    
    fprintf(out, "} %s;\n\n", cls->name);
    
    // Generate methods
    JawaMethod *method = cls->methods;
    while (method) {
        fprintf(out, "%s %s_%s(%s* this", method->return_type, cls->name, method->name, cls->name);
        if (strlen(method->params) > 0) {
            fprintf(out, ", %s", method->params);
        }
        fprintf(out, ") {\n");
        
        // Process method body line by line
        char body_copy[2048];
        strcpy(body_copy, method->body);
        
        char *line = strtok(body_copy, "\n");
        while (line) {
            // Skip closing braces that are part of method boundary  
            if (strcmp(line, "}") == 0 || strcmp(line, "    }") == 0) {
                line = strtok(NULL, "\n");
                continue;
            }
            
            char processed_line[512];
            strcpy(processed_line, line);
            
            // Simple replacements for OOP context
            // Replace 'bali ' with 'return ' at start of line
            if (strncmp(processed_line, "bali ", 5) == 0) {
                char temp[512];
                strcpy(temp, "return ");
                strcat(temp, processed_line + 5);
                strcpy(processed_line, temp);
            }
            
            // Replace 'this.' with 'this->'
            char *dot_pos = processed_line;
            while ((dot_pos = strstr(dot_pos, "this.")) != NULL) {
                memmove(dot_pos + 6, dot_pos + 5, strlen(dot_pos) - 4);
                memcpy(dot_pos + 4, "->", 2);
                dot_pos += 6;
            }
            
            // Write processed line with proper indentation
            fprintf(out, "    %s", processed_line);
            if (processed_line[strlen(processed_line)-1] != ';' && 
                processed_line[strlen(processed_line)-1] != '{' &&
                processed_line[strlen(processed_line)-1] != '}') {
                fprintf(out, ";");
            }
            fprintf(out, "\n");
            
            line = strtok(NULL, "\n");
        }
        
        fprintf(out, "}\n\n");
        method = method->next;
    }
}

void generate_object_c_code(FILE *out) {
    if (!out) return;
    
    // Generate all class definitions
    JawaClass *cls = g_classes;
    while (cls) {
        generate_class_c_code(out, cls);
        cls = cls->next;
    }
}
