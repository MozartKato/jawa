#ifndef CLASS_ENHANCED_H
#define CLASS_ENHANCED_H

#include <stdbool.h>
#include "oop/class.h" // Include original class.h for type definitions

// Forward declarations
typedef struct JawaObjectEx JawaObjectEx;

// Tipe nilai yang bisa disimpan
typedef enum {
    VAL_NULL,
    VAL_INT,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_STRING,
    VAL_OBJECT
} ValueType;

// Struktur nilai umum
typedef struct JawaValueEx {
    ValueType type;
    union {
        int int_val;
        double double_val;
        bool bool_val;
        char* string_val;
        JawaObjectEx* obj_val;
    };
} JawaValueEx;

// Property dalam objek
typedef struct ObjectProperty {
    char name[64];
    PropertyType type;  // Using PropertyType from class.h
    bool is_private;
    union {
        int int_val;
        double double_val;
        bool bool_val;
        char *string_val;
        struct JawaObjectEx *obj_val;
    } value;
    struct ObjectProperty *next;
} ObjectProperty;

// Objek dari kelas tertentu
typedef struct JawaObjectEx {
    JawaClass *class_def;  // Using JawaClass from class.h
    ObjectProperty *properties;
    struct JawaObjectEx *next; // Untuk tracking/garbage collection
} JawaObjectEx;

// Mencari kelas berdasarkan nama
JawaClass* class_find_ex(const char *name);

// Mendaftarkan kelas baru
JawaClass* class_register_ex(const char *name);

// Menambahkan properti ke kelas
void class_add_property_ex(JawaClass *cls, const char *name, PropertyType type, bool is_private);

// Menambahkan metode ke kelas
void class_add_method_ex(JawaClass *cls, const char *name, const char *return_type);

// Membuat hubungan inheritance antar kelas
void class_inherit_ex(JawaClass *child, const char *parent_name);

// Alokasi object baru
JawaObjectEx* object_create_ex(const char *class_name);

// Menghancurkan objek dan membebaskan memori
void object_destroy_ex(JawaObjectEx *obj);

// Memanggil metode pada objek
JawaValueEx object_call_method_ex(JawaObjectEx *obj, const char *method_name, JawaValueEx *args);

// Mendapatkan property dari objek
JawaValueEx object_get_property_ex(JawaObjectEx *obj, const char *prop_name);

// Mengatur property pada objek
void object_set_property_ex(JawaObjectEx *obj, const char *prop_name, JawaValueEx value);

// Fungsi untuk melakukan garbage collection manual
void object_collect_garbage_ex();

// Untuk debugging
void object_print_ex(JawaObjectEx *obj);

#endif // CLASS_ENHANCED_H
