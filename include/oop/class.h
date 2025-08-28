#ifndef OOP_CLASS_H
#define OOP_CLASS_H

#include <stdbool.h>
#include <stdio.h>

// Forward declarations
typedef struct JawaClass JawaClass;
typedef struct JawaMethod JawaMethod; 
typedef struct JawaObject JawaObject;
typedef struct JawaProperty JawaProperty;

// Property types
typedef enum {
    PROP_INT,
    PROP_DOUBLE, 
    PROP_BOOL,
    PROP_STRING,
    PROP_OBJECT
} PropertyType;

// Property definition
typedef struct JawaProperty {
    char name[64];
    PropertyType type;
    bool is_private;
    struct JawaProperty *next;
} JawaProperty;

// Method definition  
typedef struct JawaMethod {
    char name[64];
    char return_type[32];
    char params[256];
    char body[2048];
    bool is_private;
    bool is_constructor;
    struct JawaMethod *next;
} JawaMethod;

// Class definition
typedef struct JawaClass {
    char name[64];
    JawaProperty *properties;
    JawaMethod *methods;
    struct JawaClass *parent;
    struct JawaClass *next;
} JawaClass;

// Object instance
typedef struct JawaObject {
    JawaClass *class_def;
    void *data; // Property values
    struct JawaObject *next;
} JawaObject;

// Global class registry
extern JawaClass *g_classes;

// Function declarations
JawaClass* class_create(const char *name);
void class_add_property(JawaClass *cls, const char *name, PropertyType type, bool is_private);
void class_add_method(JawaClass *cls, const char *name, const char *return_type, 
                     const char *params, const char *body, bool is_private);
void class_add_constructor(JawaClass *cls, const char *params, const char *body);
JawaClass* class_find(const char *name);
void class_set_parent(JawaClass *cls, JawaClass *parent);

JawaObject* object_create(JawaClass *cls);
void object_destroy(JawaObject *obj);
void* object_get_property(JawaObject *obj, const char *name);
void object_set_property(JawaObject *obj, const char *name, void *value);

// Code generation
void generate_class_c_code(FILE *out, JawaClass *cls);
void generate_object_c_code(FILE *out);

#endif // OOP_CLASS_H
