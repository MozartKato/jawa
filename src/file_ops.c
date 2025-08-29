#include "file_ops.h"
#include "string_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>  // For access()

// Access mode constants for portability
#ifndef F_OK
#define F_OK 0  /* Test for existence.  */
#endif

#ifndef R_OK
#define R_OK 4  /* Test for read permission.  */
#endif

#ifndef W_OK
#define W_OK 2  /* Test for write permission.  */
#endif

#ifndef X_OK
#define X_OK 1  /* Test for execute permission.  */
#endif

// Utility function to set error message
static void set_file_error(JawaFile *jf, FileErrorCode code, const char *message) {
    if (!jf) return;
    
    jf->last_error = code;
    
    if (jf->error_message) {
        free(jf->error_message);
        jf->error_message = NULL;
    }
    
    if (message) {
        jf->error_message = strdup(message);
    }
}

// Convert Java mode string to C file mode
static const char* convert_mode(const char *jawa_mode) {
    if (strcmp(jawa_mode, "waca") == 0) return "r";
    if (strcmp(jawa_mode, "tulis") == 0) return "w";
    if (strcmp(jawa_mode, "tambah") == 0) return "a";
    if (strcmp(jawa_mode, "waca_plus") == 0) return "r+";
    if (strcmp(jawa_mode, "tulis_plus") == 0) return "w+";
    if (strcmp(jawa_mode, "tambah_plus") == 0) return "a+";
    if (strcmp(jawa_mode, "waca_binary") == 0) return "rb";
    if (strcmp(jawa_mode, "tulis_binary") == 0) return "wb";
    if (strcmp(jawa_mode, "tambah_binary") == 0) return "ab";
    
    // If not found, return as is (already in C format)
    return jawa_mode;
}

// Membuka file
JawaFile* file_buka(const char *path, const char *mode) {
    if (!path || !mode) {
        return NULL;
    }
    
    const char *c_mode = convert_mode(mode);
    FILE *f = fopen(path, c_mode);
    
    JawaFile *jf = malloc(sizeof(JawaFile));
    if (!jf) {
        if (f) fclose(f);
        return NULL;
    }
    
    // Initialize all fields
    jf->file = f;
    jf->mode = strdup(mode);
    jf->path = strdup(path);
    jf->is_open = (f != NULL);
    jf->last_error = jf->is_open ? FILE_OK : FILE_NOT_FOUND;
    jf->error_message = NULL;
    
    if (!jf->is_open) {
        char error_buffer[256];
        snprintf(error_buffer, sizeof(error_buffer), "Could not open file '%s': %s", 
                 path, strerror(errno));
        set_file_error(jf, FILE_NOT_FOUND, error_buffer);
        fprintf(stderr, "Error: %s\n", error_buffer);
    }
    
    return jf;
}

// Menutup file
void file_tutup(JawaFile *jf) {
    if (!jf) return;
    
    if (jf->file && jf->is_open) {
        fclose((FILE*)jf->file);
        jf->file = NULL;
        jf->is_open = false;
    }
    
    if (jf->mode) {
        free(jf->mode);
        jf->mode = NULL;
    }
    
    if (jf->path) {
        free(jf->path);
        jf->path = NULL;
    }
    
    if (jf->error_message) {
        free(jf->error_message);
        jf->error_message = NULL;
    }
    
    free(jf);
}

// Membaca seluruh isi file sebagai string
char* file_waca_kabeh(JawaFile *jf) {
    if (!jf || !jf->file || !jf->is_open) {
        if (jf) {
            set_file_error(jf, FILE_INVALID_OPERATION, "Cannot read from closed or invalid file");
        }
        return NULL;
    }
    
    FILE *f = (FILE*)jf->file;
    
    // Simpan posisi saat ini
    long current_pos = ftell(f);
    
    // Cari tahu ukuran file
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size <= 0) {
        // Empty file or error
        fseek(f, current_pos, SEEK_SET); // Restore position
        return strdup("");
    }
    
    // Alokasi buffer
    char *buffer = malloc(size + 1);
    if (!buffer) {
        set_file_error(jf, FILE_OUT_OF_MEMORY, "Could not allocate memory for file content");
        fseek(f, current_pos, SEEK_SET); // Restore position
        return NULL;
    }
    
    // Baca file
    size_t read_size = fread(buffer, 1, size, f);
    buffer[read_size] = '\0';
    
    // Kembalikan ke posisi semula
    fseek(f, current_pos, SEEK_SET);
    
    return buffer;
}

// Membaca file baris per baris
char* file_waca_baris(JawaFile *jf) {
    if (!jf || !jf->file || !jf->is_open) {
        if (jf) {
            set_file_error(jf, FILE_INVALID_OPERATION, "Cannot read from closed or invalid file");
        }
        return NULL;
    }
    
    FILE *f = (FILE*)jf->file;
    
    char buffer[4096]; // Use a larger buffer
    if (fgets(buffer, sizeof(buffer), f)) {
        // Hapus newline jika ada
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            // Also remove \r if present (for Windows files)
            if (len > 1 && buffer[len-2] == '\r') {
                buffer[len-2] = '\0';
            }
        }
        return strdup(buffer);
    }
    
    // EOF atau error
    if (ferror(f)) {
        set_file_error(jf, FILE_UNKNOWN_ERROR, "Error reading line from file");
    }
    return NULL;
}

// Menulis string ke file
int file_tulis(JawaFile *jf, const char *text) {
    if (!jf || !jf->file || !jf->is_open) {
        if (jf) {
            set_file_error(jf, FILE_INVALID_OPERATION, "Cannot write to closed or invalid file");
        }
        return 0;
    }
    
    if (!text) {
        set_file_error(jf, FILE_INVALID_ARGUMENT, "Null text argument");
        return 0;
    }
    
    FILE *f = (FILE*)jf->file;
    int result = fputs(text, f);
    
    if (result < 0) {
        set_file_error(jf, FILE_UNKNOWN_ERROR, "Error writing to file");
        return 0;
    }
    
    fflush(f);
    return 1; // Success
}

// Menulis string ke file dengan newline
int file_tulis_baris(JawaFile *jf, const char *text) {
    if (!jf || !jf->file || !jf->is_open) {
        if (jf) {
            set_file_error(jf, FILE_INVALID_OPERATION, "Cannot write to closed or invalid file");
        }
        return 0;
    }
    
    if (!text) {
        set_file_error(jf, FILE_INVALID_ARGUMENT, "Null text argument");
        return 0;
    }
    
    FILE *f = (FILE*)jf->file;
    int result = fprintf(f, "%s\n", text);
    
    if (result < 0) {
        set_file_error(jf, FILE_UNKNOWN_ERROR, "Error writing line to file");
        return 0;
    }
    
    fflush(f);
    return 1; // Success
}

// Mendapatkan informasi file
FileInfo* file_info(const char *path) {
    if (!path) return NULL;
    
    struct stat st;
    if (stat(path, &st) != 0) {
        return NULL;
    }
    
    FileInfo *info = malloc(sizeof(FileInfo));
    if (!info) return NULL;
    
    // Get file name from path
    const char *filename = path;
    const char *last_slash = strrchr(path, '/');
    if (last_slash) {
        filename = last_slash + 1;
    }
    
    info->name = strdup(filename);
    info->full_path = strdup(path);
    info->size = st.st_size;
    info->exists = true;
    info->is_readable = (access(path, R_OK) == 0);
    info->is_writable = (access(path, W_OK) == 0);
    
    return info;
}

// Mengecek keberadaan file
bool file_ada(const char *path) {
    if (!path) return false;
    return (access(path, F_OK) == 0);
}

// Menghapus file
bool file_hapus(const char *path) {
    if (!path) return false;
    return (remove(path) == 0);
}

// Mengubah nama file
bool file_ubah_nama(const char *old_path, const char *new_path) {
    if (!old_path || !new_path) return false;
    return (rename(old_path, new_path) == 0);
}

// Mengubah posisi pointer file
bool file_ubah_posisi(JawaFile *jf, long offset, int whence) {
    if (!jf || !jf->file || !jf->is_open) return false;
    
    FILE *f = (FILE*)jf->file;
    return (fseek(f, offset, whence) == 0);
}

// Mendapatkan posisi pointer file
long file_posisi(JawaFile *jf) {
    if (!jf || !jf->file || !jf->is_open) return -1;
    
    FILE *f = (FILE*)jf->file;
    return ftell(f);
}

// Membaca sejumlah byte dari file
char* file_waca_bytes(JawaFile *jf, size_t bytes) {
    if (!jf || !jf->file || !jf->is_open || bytes <= 0) {
        if (jf) {
            set_file_error(jf, FILE_INVALID_OPERATION, "Cannot read bytes from closed or invalid file");
        }
        return NULL;
    }
    
    FILE *f = (FILE*)jf->file;
    
    // Alokasi buffer
    char *buffer = malloc(bytes + 1);
    if (!buffer) {
        set_file_error(jf, FILE_OUT_OF_MEMORY, "Could not allocate memory for reading bytes");
        return NULL;
    }
    
    // Baca bytes
    size_t read_size = fread(buffer, 1, bytes, f);
    buffer[read_size] = '\0';
    
    return buffer;
}

// Mendapatkan error terakhir
const char* file_error_message(JawaFile *jf) {
    if (!jf || !jf->error_message) return "No error information";
    return jf->error_message;
}

// Mendapatkan kode error terakhir
FileErrorCode file_error_code(JawaFile *jf) {
    if (!jf) return FILE_UNKNOWN_ERROR;
    return jf->last_error;
}

// Mengecek apakah file bisa dibaca
bool file_bisa_dibaca(const char *path) {
    if (!path) return false;
    return (access(path, R_OK) == 0);
}

// Mengecek apakah file bisa ditulis
bool file_bisa_ditulis(const char *path) {
    if (!path) return false;
    return (access(path, W_OK) == 0);
}

// Membuat direktori
bool file_buat_direktori(const char *path) {
    if (!path) return false;
    
#ifdef _WIN32
    return (_mkdir(path) == 0);
#else
    return (mkdir(path, 0777) == 0);
#endif
}

// Mengecek apakah path adalah direktori
bool file_adalah_direktori(const char *path) {
    if (!path) return false;
    
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    
    return S_ISDIR(st.st_mode);
}

// Mendapatkan list file dalam direktori
char** file_list_direktori(const char *path, int *count) {
    if (!path || !count) return NULL;
    
    DIR *dir = opendir(path);
    if (!dir) {
        *count = 0;
        return NULL;
    }
    
    // Count entries first
    int entry_count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            entry_count++;
        }
    }
    
    // Allocate array for result
    char **result = malloc(sizeof(char*) * (entry_count + 1));
    if (!result) {
        closedir(dir);
        *count = 0;
        return NULL;
    }
    
    // Reset directory
    rewinddir(dir);
    
    // Fill array
    int i = 0;
    while ((entry = readdir(dir)) != NULL && i < entry_count) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            result[i] = strdup(entry->d_name);
            i++;
        }
    }
    
    result[i] = NULL; // Null-terminate array
    *count = i;
    
    closedir(dir);
    return result;
}
// Cek apakah sudah di akhir file
int file_akhir(JawaFile *jf) {
    if (!jf || !jf->file || !jf->is_open) {
        return 1; // Treated as EOF if file invalid
    }
    
    FILE *f = (FILE*)jf->file;
    return feof(f);
}
