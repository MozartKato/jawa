#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

// Struktur untuk menyimpan informasi paket
typedef struct {
    char *nama;
    char *versi;
    char *deskripsi;
    char *penulis;
    char **file_daftar;
    int file_count;
    char *entry_point;
    char **dependencies;
    int dep_count;
} JawaPaket;

// Fungsi utama untuk menangani perintah paket
int handle_paket_command(int argc, char **argv);

// Fungsi untuk memasang paket
int pasang_paket(const char *nama_paket, const char *versi);

// Dapatkan jalur direktori paket Jawa
char *get_jawa_packages_dir();

// Parse file konfigurasi paket
JawaPaket *parse_paket_json(const char *json_path);

// Fungsi bantuan untuk resolusi impor
char *resolve_import_path(const char *base_dir, const char *import_path);

// Bebaskan memori paket
void free_paket(JawaPaket *paket);

#endif /* PACKAGE_MANAGER_H */
