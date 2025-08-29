#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stdbool.h>
#include <stddef.h>

// Error codes
typedef enum {
    FILE_OK = 0,
    FILE_NOT_FOUND,
    FILE_ACCESS_DENIED,
    FILE_ALREADY_EXISTS,
    FILE_UNKNOWN_ERROR,
    FILE_INVALID_OPERATION,
    FILE_OUT_OF_MEMORY,
    FILE_INVALID_ARGUMENT
} FileErrorCode;

// File mode constants
#define FILE_MODE_READ      "r"   // "waca"
#define FILE_MODE_WRITE     "w"   // "tulis"
#define FILE_MODE_APPEND    "a"   // "tambah"
#define FILE_MODE_READ_PLUS "r+"  // "waca_plus"
#define FILE_MODE_WRITE_PLUS "w+" // "tulis_plus"
#define FILE_MODE_APPEND_PLUS "a+" // "tambah_plus"
#define FILE_MODE_BINARY    "b"   // Modifier for binary

// Seek mode constants
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Struktur untuk file handler
typedef struct {
    void *file;        // FILE* pointer
    char *mode;        // Mode akses
    char *path;        // Path file
    bool is_open;      // Status file (terbuka/tertutup)
    FileErrorCode last_error; // Kode error terakhir
    char *error_message;      // Pesan error terakhir
} JawaFile;

// Informasi file
typedef struct {
    char *name;        // Nama file
    char *full_path;   // Path lengkap
    long size;         // Ukuran file dalam bytes
    bool exists;       // Apakah file ada
    bool is_readable;  // Apakah file bisa dibaca
    bool is_writable;  // Apakah file bisa ditulis
} FileInfo;

// Membuka file
// mode: "waca" (read), "tulis" (write), "tambah" (append), etc.
JawaFile* file_buka(const char *path, const char *mode);

// Menutup file
void file_tutup(JawaFile *jf);

// Membaca seluruh isi file sebagai string
char* file_waca_kabeh(JawaFile *jf);

// Membaca file baris per baris
char* file_waca_baris(JawaFile *jf);

// Menulis string ke file
int file_tulis(JawaFile *jf, const char *text);

// Menulis string ke file dengan newline
int file_tulis_baris(JawaFile *jf, const char *text);

// Cek apakah sudah di akhir file
int file_akhir(JawaFile *jf);

// === New functions ===

// Mendapatkan informasi file
FileInfo* file_info(const char *path);

// Mengecek keberadaan file
bool file_ada(const char *path);

// Menghapus file
bool file_hapus(const char *path);

// Mengubah nama file
bool file_ubah_nama(const char *old_path, const char *new_path);

// Mengubah posisi pointer file
bool file_ubah_posisi(JawaFile *jf, long offset, int whence);

// Mendapatkan posisi pointer file
long file_posisi(JawaFile *jf);

// Membaca sejumlah byte dari file
char* file_waca_bytes(JawaFile *jf, size_t bytes);

// Mendapatkan error terakhir
const char* file_error_message(JawaFile *jf);

// Mendapatkan kode error terakhir
FileErrorCode file_error_code(JawaFile *jf);

// Mengecek apakah file bisa dibaca
bool file_bisa_dibaca(const char *path);

// Mengecek apakah file bisa ditulis
bool file_bisa_ditulis(const char *path);

// Membuat direktori
bool file_buat_direktori(const char *path);

// Mengecek apakah path adalah direktori
bool file_adalah_direktori(const char *path);

// Mendapatkan list file dalam direktori
char** file_list_direktori(const char *path, int *count);

#endif // FILE_OPS_H
