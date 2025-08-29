#ifndef STRING_ENHANCED_H
#define STRING_ENHANCED_H

// Menghapus spasi dari kedua sisi string
// resep → resep (tidak berubah)
// " resep " → "resep"
char* string_trim_ex(const char* str);

// Mengubah string menjadi array/daftar karakter
// "Jawa" → ["J", "a", "w", "a"]
char** string_ke_karakter(const char* str, int* count);

// Mengecek apakah string berisi substring
// "Bahasa Jawa" ngandhut "Jawa" → true
int string_ngandhut(const char* str, const char* substr);

// Menghitung kemunculan substring dalam string
// "Bahasa Jawa dan budaya Jawa" ngitung "Jawa" → 2
int string_ngitung(const char* str, const char* substr);

// Mengecek apakah string dimulai dengan prefix
// "Bahasa Jawa" miwiti "Bahasa" → true
int string_miwiti(const char* str, const char* prefix);

// Mengecek apakah string diakhiri dengan suffix
// "Bahasa Jawa" mungkasi "Jawa" → true
int string_mungkasi(const char* str, const char* suffix);

// Menggabungkan array string dengan delimiter
// ["Jawa", "Sunda", "Bali"] + ", " → "Jawa, Sunda, Bali"
char* string_gabung_array(char** arr, int count, const char* delimiter);

// Mengulang string sebanyak n kali
// "Jawa" ulang 3 → "JawaJawaJawa"
char* string_ulang(const char* str, int count);

// Mengonversi string ke format JSON
// "Jawa" → "\"Jawa\""
char* string_ke_json(const char* str);

#endif // STRING_ENHANCED_H
