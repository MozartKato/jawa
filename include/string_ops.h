#ifndef STRING_OPS_H
#define STRING_OPS_H

#include <stdio.h>
#include <stdbool.h>

// String operations
char* string_concat(const char* a, const char* b);
char** string_split(const char* str, const char* delimiter, int* count);
char* string_replace(const char* str, const char* find, const char* replace);
int string_length(const char* str);
char* string_substring(const char* str, int start, int length);
char* string_to_upper(const char* str);
char* string_to_lower(const char* str);
bool string_contains(const char* str, const char* substr);
char* string_trim(const char* str);

// Array/List operations
typedef struct {
    void** data;
    int size;
    int capacity;
    int element_size;
} JawaArray;

JawaArray* array_create(int element_size);
void array_add(JawaArray* arr, void* element);
void* array_get(JawaArray* arr, int index);
void array_set(JawaArray* arr, int index, void* element);
int array_size(JawaArray* arr);
void array_free(JawaArray* arr);

// Map/Object operations
typedef struct MapNode {
    char* key;
    void* value;
    struct MapNode* next;
} MapNode;

typedef struct {
    MapNode** buckets;
    int size;
    int capacity;
} JawaMap;

JawaMap* map_create();
void map_set(JawaMap* map, const char* key, void* value);
void* map_get(JawaMap* map, const char* key);
bool map_has(JawaMap* map, const char* key);
void map_free(JawaMap* map);

#endif // STRING_OPS_H
