#include "string_ops.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// String operations implementation
char* string_concat(const char* a, const char* b) {
    if (!a || !b) return NULL;
    
    int len_a = strlen(a);
    int len_b = strlen(b);
    char* result = malloc(len_a + len_b + 1);
    
    if (!result) return NULL;
    
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char** string_split(const char* str, const char* delimiter, int* count) {
    if (!str || !delimiter || !count) return NULL;
    
    char* temp = strdup(str);
    if (!temp) return NULL;
    
    // Count tokens first
    *count = 0;
    char* token = strtok(temp, delimiter);
    while (token) {
        (*count)++;
        token = strtok(NULL, delimiter);
    }
    
    free(temp);
    if (*count == 0) return NULL;
    
    // Allocate result array
    char** result = malloc(*count * sizeof(char*));
    if (!result) return NULL;
    
    // Split again and store
    temp = strdup(str);
    token = strtok(temp, delimiter);
    int i = 0;
    while (token && i < *count) {
        result[i] = strdup(token);
        token = strtok(NULL, delimiter);
        i++;
    }
    
    free(temp);
    return result;
}

char* string_replace(const char* str, const char* find, const char* replace) {
    if (!str || !find || !replace) return NULL;
    
    int find_len = strlen(find);
    int replace_len = strlen(replace);
    int str_len = strlen(str);
    
    // Count occurrences
    int count = 0;
    const char* p = str;
    while ((p = strstr(p, find)) != NULL) {
        count++;
        p += find_len;
    }
    
    if (count == 0) return strdup(str);
    
    // Calculate new length
    int new_len = str_len + count * (replace_len - find_len);
    char* result = malloc(new_len + 1);
    if (!result) return NULL;
    
    // Perform replacement
    const char* src = str;
    char* dst = result;
    while ((p = strstr(src, find)) != NULL) {
        int len_before = p - src;
        strncpy(dst, src, len_before);
        dst += len_before;
        strcpy(dst, replace);
        dst += replace_len;
        src = p + find_len;
    }
    strcpy(dst, src);
    
    return result;
}

int string_length(const char* str) {
    return str ? strlen(str) : 0;
}

char* string_substring(const char* str, int start, int length) {
    if (!str || start < 0 || length < 0) return NULL;
    
    int str_len = strlen(str);
    if (start >= str_len) return strdup("");
    
    if (start + length > str_len) {
        length = str_len - start;
    }
    
    char* result = malloc(length + 1);
    if (!result) return NULL;
    
    strncpy(result, str + start, length);
    result[length] = '\0';
    return result;
}

char* string_to_upper(const char* str) {
    if (!str) return NULL;
    
    char* result = strdup(str);
    if (!result) return NULL;
    
    for (int i = 0; result[i]; i++) {
        result[i] = toupper(result[i]);
    }
    
    return result;
}

char* string_to_lower(const char* str) {
    if (!str) return NULL;
    
    char* result = strdup(str);
    if (!result) return NULL;
    
    for (int i = 0; result[i]; i++) {
        result[i] = tolower(result[i]);
    }
    
    return result;
}

bool string_contains(const char* str, const char* substr) {
    if (!str || !substr) return false;
    return strstr(str, substr) != NULL;
}

char* string_trim(const char* str) {
    if (!str) return NULL;
    
    // Skip leading whitespace
    while (isspace(*str)) str++;
    
    if (*str == 0) return strdup("");
    
    // Find end
    const char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    
    // Create result
    int length = end - str + 1;
    char* result = malloc(length + 1);
    if (!result) return NULL;
    
    strncpy(result, str, length);
    result[length] = '\0';
    return result;
}

// Array operations implementation
JawaArray* array_create(int element_size) {
    JawaArray* arr = malloc(sizeof(JawaArray));
    if (!arr) return NULL;
    
    arr->data = malloc(10 * sizeof(void*));
    if (!arr->data) {
        free(arr);
        return NULL;
    }
    
    arr->size = 0;
    arr->capacity = 10;
    arr->element_size = element_size;
    return arr;
}

void array_add(JawaArray* arr, void* element) {
    if (!arr || !element) return;
    
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = realloc(arr->data, arr->capacity * sizeof(void*));
        if (!arr->data) return;
    }
    
    // Allocate memory for element and copy
    void* new_element = malloc(arr->element_size);
    if (!new_element) return;
    
    memcpy(new_element, element, arr->element_size);
    arr->data[arr->size] = new_element;
    arr->size++;
}

void* array_get(JawaArray* arr, int index) {
    if (!arr || index < 0 || index >= arr->size) return NULL;
    return arr->data[index];
}

void array_set(JawaArray* arr, int index, void* element) {
    if (!arr || !element || index < 0 || index >= arr->size) return;
    
    if (arr->data[index]) {
        free(arr->data[index]);
    }
    
    void* new_element = malloc(arr->element_size);
    if (!new_element) return;
    
    memcpy(new_element, element, arr->element_size);
    arr->data[index] = new_element;
}

int array_size(JawaArray* arr) {
    return arr ? arr->size : 0;
}

void array_free(JawaArray* arr) {
    if (!arr) return;
    
    for (int i = 0; i < arr->size; i++) {
        if (arr->data[i]) {
            free(arr->data[i]);
        }
    }
    
    free(arr->data);
    free(arr);
}

// Map operations implementation
static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

JawaMap* map_create() {
    JawaMap* map = malloc(sizeof(JawaMap));
    if (!map) return NULL;
    
    map->capacity = 16;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(MapNode*));
    
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    return map;
}

void map_set(JawaMap* map, const char* key, void* value) {
    if (!map || !key) return;
    
    unsigned int index = hash_string(key) % map->capacity;
    MapNode* node = map->buckets[index];
    
    // Check if key already exists
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
        node = node->next;
    }
    
    // Create new node
    MapNode* new_node = malloc(sizeof(MapNode));
    if (!new_node) return;
    
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;
}

void* map_get(JawaMap* map, const char* key) {
    if (!map || !key) return NULL;
    
    unsigned int index = hash_string(key) % map->capacity;
    MapNode* node = map->buckets[index];
    
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    
    return NULL;
}

bool map_has(JawaMap* map, const char* key) {
    return map_get(map, key) != NULL;
}

void map_free(JawaMap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->capacity; i++) {
        MapNode* node = map->buckets[i];
        while (node) {
            MapNode* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    
    free(map->buckets);
    free(map);
}
