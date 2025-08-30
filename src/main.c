#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // For system() function
#include "native.h"
#include "package_manager.h"

#define JAWA_VERSION "0.3"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Jawa Programming Language v%s\n", JAWA_VERSION);
        printf("Usage:\n");
        printf("  %s build input.jw output_bin    - Kompilasi file .jw ke biner\n", argv[0]);
        printf("  %s mlayu input.jw                 - Jalankan file .jw tanpa kompilasi\n", argv[0]);
        printf("  %s paket <perintah> [argumen]   - Kelola paket Jawa\n", argv[0]);
        printf("  %s version                      - Tampilkan versi\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "version") == 0) {
        printf("Jawa Programming Language v%s\n", JAWA_VERSION);
        return 0;
    }
    
    if (strcmp(argv[1], "paket") == 0) {
        return handle_paket_command(argc, argv);
    }
    
    if (argc < 3) {
        printf("Jawa Programming Language v%s\n", JAWA_VERSION);
        printf("Usage:\n");
        printf("  %s build input.jw output_bin    - Kompilasi file .jw ke biner\n", argv[0]);
        printf("  %s mlayu input.jw                 - Jalankan file .jw tanpa kompilasi\n", argv[0]);
        printf("  %s paket <perintah> [argumen]   - Kelola paket Jawa\n", argv[0]);
        printf("  %s version                      - Tampilkan versi\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "build") == 0) {
        if (argc != 4) { fprintf(stderr, "build mode needs input & output\n"); return 1; }
        int rc = build_native(argv[2], argv[3]);
        return rc;
    } else if (strcmp(argv[1], "mlayu") == 0) {
        // Implementasi sederhana untuk menjalankan file langsung
        char temp_out[256];
        sprintf(temp_out, "%s.temp.bin", argv[2]);
        
        int rc = build_native(argv[2], temp_out);
        if (rc != 0) return rc;
        
        // Jalankan file yang sudah dibuild
#ifdef _WIN32
        system(temp_out);
#else
        char run_cmd[512];
        sprintf(run_cmd, "./%s", temp_out);
        system(run_cmd);
#endif
        
        // Hapus file sementara
        remove(temp_out);
        return 0;
    } else {
        fprintf(stderr, "Unknown command\n");
        return 1;
    }
    return 0;
}
