#include "string_enhanced.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Helpers for converting int to string
char* int_to_string(int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return strdup(buffer);
}

// Fungsi untuk menghapus spasi dari kedua sisi string
char* string_trim_ex(const char* str) {
    if (!str) return NULL;
    
    int len = strlen(str);
    if (len == 0) return strdup("");
    
    // Temukan awal (tanpa spasi)
    int start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }
    
    // Semua spasi
    if (start == len) return strdup("");
    
    // Temukan akhir (tanpa spasi)
    int end = len - 1;
    while (end > start && isspace((unsigned char)str[end])) {
        end--;
    }
    
    // Buat string baru
    int result_len = end - start + 1;
    char* result = malloc(result_len + 1);
    if (!result) return NULL;
    
    strncpy(result, str + start, result_len);
    result[result_len] = '\0';
    
    return result;
}

// Mengubah string menjadi array/daftar karakter
char** string_ke_karakter(const char* str, int* count) {
    if (!str || !count) return NULL;
    
    int len = strlen(str);
    *count = len;
    
    if (len == 0) return NULL;
    
    char** result = malloc(len * sizeof(char*));
    if (!result) return NULL;
    
    for (int i = 0; i < len; i++) {
        result[i] = malloc(2);
        result[i][0] = str[i];
        result[i][1] = '\0';
    }
    
    return result;
}

// Mengecek apakah string berisi substring
int string_ngandhut(const char* str, const char* substr) {
    if (!str || !substr) return 0;
    return strstr(str, substr) != NULL;
}

// Menghitung kemunculan substring dalam string
int string_ngitung(const char* str, const char* substr) {
    if (!str || !substr) return 0;
    
    int count = 0;
    const char* pos = str;
    int substr_len = strlen(substr);
    
    if (substr_len == 0) return 0;
    
    while ((pos = strstr(pos, substr)) != NULL) {
        count++;
        pos += substr_len;
    }
    
    return count;
}

// Mengecek apakah string dimulai dengan prefix
int string_miwiti(const char* str, const char* prefix) {
    if (!str || !prefix) return 0;
    
    int prefix_len = strlen(prefix);
    if (prefix_len == 0) return 1;
    
    return strncmp(str, prefix, prefix_len) == 0;
}

// Mengecek apakah string diakhiri dengan suffix
int string_mungkasi(const char* str, const char* suffix) {
    if (!str || !suffix) return 0;
    
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    if (suffix_len == 0) return 1;
    if (suffix_len > str_len) return 0;
    
    return strcmp(str + (str_len - suffix_len), suffix) == 0;
}

// Menggabungkan array string dengan delimiter
char* string_gabung_array(char** arr, int count, const char* delimiter) {
    if (!arr || count <= 0 || !delimiter) return NULL;
    
    // Hitung total panjang
    int total_len = 0;
    int delim_len = strlen(delimiter);
    
    for (int i = 0; i < count; i++) {
        if (arr[i]) {
            total_len += strlen(arr[i]);
        }
        
        // Tambahkan delimiter kecuali untuk elemen terakhir
        if (i < count - 1) {
            total_len += delim_len;
        }
    }
    
    // Alokasi hasil
    char* result = malloc(total_len + 1);
    if (!result) return NULL;
    
    result[0] = '\0';
    
    // Gabungkan string
    for (int i = 0; i < count; i++) {
        if (arr[i]) {
            strcat(result, arr[i]);
        }
        
        // Tambahkan delimiter kecuali untuk elemen terakhir
        if (i < count - 1) {
            strcat(result, delimiter);
        }
    }
    
    return result;
}

// Mengulang string sebanyak n kali
char* string_ulang(const char* str, int count) {
    if (!str || count <= 0) return NULL;
    
    int str_len = strlen(str);
    int total_len = str_len * count;
    
    char* result = malloc(total_len + 1);
    if (!result) return NULL;
    
    result[0] = '\0';
    
    for (int i = 0; i < count; i++) {
        strcat(result, str);
    }
    
    return result;
}

// Mengonversi string ke format JSON
char* string_ke_json(const char* str) {
    if (!str) return NULL;
    
    int str_len = strlen(str);
    
    // Alokasi dengan ukuran maksimal (kasus terburuk: semua karakter perlu escape)
    char* result = malloc(str_len * 2 + 3); // +3 untuk quotes dan null terminator
    if (!result) return NULL;
    
    int j = 0;
    result[j++] = '"';
    
    for (int i = 0; i < str_len; i++) {
        char c = str[i];
        switch (c) {
            case '\\': result[j++] = '\\'; result[j++] = '\\'; break;
            case '"': result[j++] = '\\'; result[j++] = '"'; break;
            case '\n': result[j++] = '\\'; result[j++] = 'n'; break;
            case '\r': result[j++] = '\\'; result[j++] = 'r'; break;
            case '\t': result[j++] = '\\'; result[j++] = 't'; break;
            default: result[j++] = c;
        }
    }
    
    result[j++] = '"';
    result[j] = '\0';
    
    return result;
}
