#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "common.h"
#include "parser.h"
#include "lexer.h"
#include "native.h"
#include "statements.h"

#ifdef _WIN32
#include <windows.h>
#define JAWA_HOME_ENV "JAWA_HOME"
#define PATH_SEPARATOR "\\"
#define mkdir(path, mode) mkdir(path)
#elif defined(__APPLE__) || defined(__linux__)
#define JAWA_HOME_ENV "JAWA_HOME"
#define PATH_SEPARATOR "/"
#endif

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

// Fungsi untuk mengunduh file dari URL
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Unduh file dari URL
int unduh_file(const char *url, const char *output_path) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Gagal menginisialisasi curl\n");
        return 0;
    }
    
    fp = fopen(output_path, "wb");
    if (!fp) {
        fprintf(stderr, "Gagal membuka file untuk menulis: %s\n", output_path);
        curl_easy_cleanup(curl);
        return 0;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Gagal mengunduh: %s\n", curl_easy_strerror(res));
        fclose(fp);
        curl_easy_cleanup(curl);
        return 0;
    }
    
    fclose(fp);
    curl_easy_cleanup(curl);
    return 1;
}

// Dapatkan jalur direktori paket Jawa
char *get_jawa_packages_dir() {
    char *home_dir = getenv(JAWA_HOME_ENV);
    char *packages_dir;
    
    if (!home_dir) {
        // Fallback ke direktori pengguna jika JAWA_HOME tidak diset
        home_dir = getenv("HOME");
        if (!home_dir) {
            fprintf(stderr, "Gagal mendapatkan direktori home\n");
            return NULL;
        }
    }
    
    packages_dir = malloc(strlen(home_dir) + 20);
    if (!packages_dir) {
        fprintf(stderr, "Gagal mengalokasikan memori\n");
        return NULL;
    }
    
    sprintf(packages_dir, "%s%s.jawa%spaket", home_dir, PATH_SEPARATOR, PATH_SEPARATOR);
    
    // Buat direktori jika belum ada
    struct stat st = {0};
    char *jawa_dir = malloc(strlen(home_dir) + 10);
    sprintf(jawa_dir, "%s%s.jawa", home_dir, PATH_SEPARATOR);
    
    if (stat(jawa_dir, &st) == -1) {
#ifdef _WIN32
        mkdir(jawa_dir);
#else
        mkdir(jawa_dir, 0700);
#endif
    }
    
    if (stat(packages_dir, &st) == -1) {
#ifdef _WIN32
        mkdir(packages_dir);
#else
        mkdir(packages_dir, 0700);
#endif
    }
    
    free(jawa_dir);
    return packages_dir;
}

// Parse file konfigurasi paket
JawaPaket *parse_paket_json(const char *json_path) {
    FILE *fp;
    struct json_object *parsed_json;
    struct json_object *nama, *versi, *deskripsi, *penulis, *berkas, *titik_masuk, *ketergantungan;
    
    fp = fopen(json_path, "r");
    if (!fp) {
        fprintf(stderr, "Gagal membuka file paket.json: %s\n", json_path);
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *json_str = malloc(file_size + 1);
    if (!json_str) {
        fprintf(stderr, "Gagal mengalokasikan memori\n");
        fclose(fp);
        return NULL;
    }
    
    fread(json_str, 1, file_size, fp);
    fclose(fp);
    json_str[file_size] = '\0';
    
    parsed_json = json_tokener_parse(json_str);
    if (!parsed_json) {
        fprintf(stderr, "Gagal mengurai JSON\n");
        free(json_str);
        return NULL;
    }
    
    JawaPaket *paket = malloc(sizeof(JawaPaket));
    if (!paket) {
        fprintf(stderr, "Gagal mengalokasikan memori untuk paket\n");
        json_object_put(parsed_json);
        free(json_str);
        return NULL;
    }
    
    // Mengambil properti dasar
    json_object_object_get_ex(parsed_json, "nama", &nama);
    json_object_object_get_ex(parsed_json, "versi", &versi);
    json_object_object_get_ex(parsed_json, "deskripsi", &deskripsi);
    json_object_object_get_ex(parsed_json, "penulis", &penulis);
    json_object_object_get_ex(parsed_json, "berkas", &berkas);
    json_object_object_get_ex(parsed_json, "titik_masuk", &titik_masuk);
    
    paket->nama = strdup(json_object_get_string(nama));
    paket->versi = strdup(json_object_get_string(versi));
    paket->deskripsi = strdup(json_object_get_string(deskripsi));
    paket->penulis = strdup(json_object_get_string(penulis));
    paket->entry_point = strdup(json_object_get_string(titik_masuk));
    
    // Mengambil daftar berkas
    int berkas_count = json_object_array_length(berkas);
    paket->file_count = berkas_count;
    paket->file_daftar = malloc(sizeof(char*) * berkas_count);
    
    for (int i = 0; i < berkas_count; i++) {
        struct json_object *file_obj = json_object_array_get_idx(berkas, i);
        paket->file_daftar[i] = strdup(json_object_get_string(file_obj));
    }
    
    // Mengambil ketergantungan
    if (json_object_object_get_ex(parsed_json, "ketergantungan", &ketergantungan)) {
        // Count dependencies and allocate space
        int dep_count = 0;
        struct json_object_iterator it_end = json_object_iter_end(ketergantungan);
        struct json_object_iterator it = json_object_iter_begin(ketergantungan);
        
        while (!json_object_iter_equal(&it, &it_end)) {
            dep_count++;
            json_object_iter_next(&it);
        }
        
        paket->dep_count = dep_count;
        paket->dependencies = malloc(sizeof(char*) * dep_count);
        
        // Now iterate again to get the values
        int idx = 0;
        it = json_object_iter_begin(ketergantungan);
        
        while (!json_object_iter_equal(&it, &it_end)) {
            const char *key = json_object_iter_peek_name(&it);
            struct json_object *val = json_object_iter_peek_value(&it);
            
            char *dep_str = malloc(strlen(key) + strlen(json_object_get_string(val)) + 2);
            sprintf(dep_str, "%s@%s", key, json_object_get_string(val));
            paket->dependencies[idx++] = dep_str;
            
            json_object_iter_next(&it);
        }
    } else {
        paket->dep_count = 0;
        paket->dependencies = NULL;
    }
    
    json_object_put(parsed_json);
    free(json_str);
    return paket;
}

// Pasang paket
int pasang_paket(const char *nama_paket, const char *versi) {
    char *packages_dir = get_jawa_packages_dir();
    if (!packages_dir) {
        return 0;
    }
    
    // Buat URL untuk mengunduh paket (contoh implementasi)
    char url[512];
    sprintf(url, "https://jawa-packages.example.com/%s/%s.zip", nama_paket, versi ? versi : "latest");
    
    // Buat path output
    char output_path[512];
    sprintf(output_path, "%s%s%s-%s.zip", packages_dir, PATH_SEPARATOR, nama_paket, versi ? versi : "latest");
    
    printf("Memasang paket %s (versi %s)...\n", nama_paket, versi ? versi : "latest");
    
    // Unduh paket
    if (!unduh_file(url, output_path)) {
        fprintf(stderr, "Gagal mengunduh paket\n");
        free(packages_dir);
        return 0;
    }
    
    // Ekstrak paket (implementasi sederhana)
    char extract_cmd[1024];
#ifdef _WIN32
    sprintf(extract_cmd, "powershell -command \"Expand-Archive -Path '%s' -DestinationPath '%s%s%s'\"", 
            output_path, packages_dir, PATH_SEPARATOR, nama_paket);
#else
    sprintf(extract_cmd, "unzip -o '%s' -d '%s%s%s'", 
            output_path, packages_dir, PATH_SEPARATOR, nama_paket);
#endif
    
    int result = system(extract_cmd);
    if (result != 0) {
        fprintf(stderr, "Gagal mengekstrak paket\n");
        free(packages_dir);
        return 0;
    }
    
    // Hapus file zip
    remove(output_path);
    
    printf("Paket %s berhasil dipasang!\n", nama_paket);
    free(packages_dir);
    return 1;
}

// Fungsi untuk menangani perintah paket
int handle_paket_command(int argc, char **argv) {
    if (argc < 3) {
        printf("Penggunaan: jawa paket <perintah> [argumen...]\n");
        printf("Perintah yang tersedia:\n");
        printf("  pasang <nama_paket[@versi]> - Pasang paket\n");
        printf("  hapus <nama_paket> - Hapus paket\n");
        printf("  daftar - Tampilkan daftar paket terpasang\n");
        printf("  init - Inisialisasi paket baru\n");
        return 0;
    }
    
    const char *command = argv[2];
    
    if (strcmp(command, "pasang") == 0) {
        if (argc < 4) {
            printf("Penggunaan: jawa paket pasang <nama_paket[@versi]>\n");
            return 0;
        }
        
        char nama_paket[256];
        char versi[64] = "latest";
        char *at_pos = strchr(argv[3], '@');
        
        if (at_pos) {
            // Format: nama_paket@versi
            int nama_len = at_pos - argv[3];
            strncpy(nama_paket, argv[3], nama_len);
            nama_paket[nama_len] = '\0';
            strcpy(versi, at_pos + 1);
        } else {
            // Hanya nama paket
            strcpy(nama_paket, argv[3]);
        }
        
        return pasang_paket(nama_paket, strcmp(versi, "latest") == 0 ? NULL : versi);
    } 
    else if (strcmp(command, "daftar") == 0) {
        char *packages_dir = get_jawa_packages_dir();
        if (!packages_dir) {
            return 0;
        }
        
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(packages_dir)) != NULL) {
            printf("Paket Terpasang:\n");
            int count = 0;
            
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    // Coba baca file paket.json untuk mendapatkan versi
                    char paket_json_path[512];
                    sprintf(paket_json_path, "%s%s%s%spaket.json", 
                            packages_dir, PATH_SEPARATOR, ent->d_name, PATH_SEPARATOR);
                    
                    FILE *fp = fopen(paket_json_path, "r");
                    if (fp) {
                        fclose(fp);
                        JawaPaket *paket = parse_paket_json(paket_json_path);
                        if (paket) {
                            printf("  - %s (versi %s): %s\n", paket->nama, paket->versi, paket->deskripsi);
                            
                            // Free memory
                            free(paket->nama);
                            free(paket->versi);
                            free(paket->deskripsi);
                            free(paket->penulis);
                            free(paket->entry_point);
                            
                            for (int i = 0; i < paket->file_count; i++) {
                                free(paket->file_daftar[i]);
                            }
                            free(paket->file_daftar);
                            
                            if (paket->dependencies) {
                                for (int i = 0; i < paket->dep_count; i++) {
                                    free(paket->dependencies[i]);
                                }
                                free(paket->dependencies);
                            }
                            
                            free(paket);
                        } else {
                            printf("  - %s (tidak dapat membaca info)\n", ent->d_name);
                        }
                    } else {
                        printf("  - %s\n", ent->d_name);
                    }
                    count++;
                }
            }
            
            if (count == 0) {
                printf("  Tidak ada paket terpasang\n");
            }
            
            closedir(dir);
        } else {
            perror("Tidak dapat membuka direktori paket");
            free(packages_dir);
            return 0;
        }
        
        free(packages_dir);
        return 1;
    }
    else if (strcmp(command, "init") == 0) {
        // Buat file paket.json di direktori saat ini
        FILE *fp = fopen("paket.json", "w");
        if (!fp) {
            fprintf(stderr, "Gagal membuat file paket.json\n");
            return 0;
        }
        
        // Dapatkan nama direktori saat ini
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            strcpy(cwd, "proyek-baru");
        } else {
            // Ekstrak nama direktori dari path
            char *last_slash = strrchr(cwd, PATH_SEPARATOR[0]);
            if (last_slash) {
                memmove(cwd, last_slash + 1, strlen(last_slash));
            }
        }
        
        // Tulis template paket.json
        fprintf(fp, "{\n");
        fprintf(fp, "    \"nama\": \"%s\",\n", cwd);
        fprintf(fp, "    \"versi\": \"0.1.0\",\n");
        fprintf(fp, "    \"deskripsi\": \"Deskripsi proyek Jawa\",\n");
        fprintf(fp, "    \"penulis\": \"Penulis\",\n");
        fprintf(fp, "    \"ketergantungan\": {\n");
        fprintf(fp, "    },\n");
        fprintf(fp, "    \"berkas\": [\n");
        fprintf(fp, "        \"src/utama.jw\"\n");
        fprintf(fp, "    ],\n");
        fprintf(fp, "    \"titik_masuk\": \"src/utama.jw\"\n");
        fprintf(fp, "}\n");
        
        fclose(fp);
        
        // Buat struktur direktori dasar
        struct stat st = {0};
        if (stat("src", &st) == -1) {
#ifdef _WIN32
            mkdir("src");
#else
            mkdir("src", 0700);
#endif
        }
        
        // Buat file utama.jw
        fp = fopen("src/utama.jw", "w");
        if (fp) {
            fprintf(fp, "// File utama untuk proyek %s\n\n", cwd);
            fprintf(fp, "fungsi utama() {\n");
            fprintf(fp, "    cetak(\"Selamat datang di proyek Jawa!\");\n");
            fprintf(fp, "}\n\n");
            fprintf(fp, "utama();\n");
            fclose(fp);
        }
        
        printf("Berhasil menginisialisasi proyek Jawa baru!\n");
        printf("Struktur proyek:\n");
        printf("  paket.json\n");
        printf("  src/\n");
        printf("    utama.jw\n");
        return 1;
    }
    else if (strcmp(command, "hapus") == 0) {
        if (argc < 4) {
            printf("Penggunaan: jawa paket hapus <nama_paket>\n");
            return 0;
        }
        
        char *packages_dir = get_jawa_packages_dir();
        if (!packages_dir) {
            return 0;
        }
        
        char package_dir[512];
        sprintf(package_dir, "%s%s%s", packages_dir, PATH_SEPARATOR, argv[3]);
        
        // Periksa apakah paket ada
        struct stat st = {0};
        if (stat(package_dir, &st) == -1) {
            fprintf(stderr, "Paket %s tidak ditemukan\n", argv[3]);
            free(packages_dir);
            return 0;
        }
        
        // Hapus direktori paket
#ifdef _WIN32
        char rm_cmd[1024];
        sprintf(rm_cmd, "rmdir /s /q \"%s\"", package_dir);
        system(rm_cmd);
#else
        char rm_cmd[1024];
        sprintf(rm_cmd, "rm -rf \"%s\"", package_dir);
        system(rm_cmd);
#endif
        
        printf("Paket %s berhasil dihapus\n", argv[3]);
        free(packages_dir);
        return 1;
    }
    else {
        printf("Perintah tidak dikenal: %s\n", command);
        return 0;
    }
}

// Fungsi bantuan untuk resolusi impor
char *resolve_import_path(const char *base_dir, const char *import_path) {
    char *resolved_path;
    
    if (import_path[0] == '.' && (import_path[1] == '/' || 
                                  (import_path[1] == '.' && import_path[2] == '/'))) {
        // Relatif impor
        resolved_path = malloc(strlen(base_dir) + strlen(import_path) + 2);
        if (!resolved_path) return NULL;
        
        if (base_dir[strlen(base_dir) - 1] == PATH_SEPARATOR[0]) {
            sprintf(resolved_path, "%s%s", base_dir, import_path + 2);
        } else {
            sprintf(resolved_path, "%s%s%s", base_dir, PATH_SEPARATOR, import_path + 2);
        }
    } else {
        // Impor paket
        char *packages_dir = get_jawa_packages_dir();
        if (!packages_dir) return NULL;
        
        resolved_path = malloc(strlen(packages_dir) + strlen(import_path) + 20);
        if (!resolved_path) {
            free(packages_dir);
            return NULL;
        }
        
        sprintf(resolved_path, "%s%s%s%sindex.jw", packages_dir, PATH_SEPARATOR, 
                import_path, PATH_SEPARATOR);
        
        free(packages_dir);
    }
    
    return resolved_path;
}

// Bebaskan memori paket
void free_paket(JawaPaket *paket) {
    if (!paket) return;
    
    free(paket->nama);
    free(paket->versi);
    free(paket->deskripsi);
    free(paket->penulis);
    free(paket->entry_point);
    
    for (int i = 0; i < paket->file_count; i++) {
        free(paket->file_daftar[i]);
    }
    free(paket->file_daftar);
    
    if (paket->dependencies) {
        for (int i = 0; i < paket->dep_count; i++) {
            free(paket->dependencies[i]);
        }
        free(paket->dependencies);
    }
    
    free(paket);
}
