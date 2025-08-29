# Demonstrasi Sistem Paket Jawa

Direktori ini berisi contoh penggunaan sistem paket untuk bahasa pemrograman Jawa.

## Struktur Contoh

- `paket.json` - File konfigurasi paket
- `src/utama.jw` - File program utama
- `src/pembantu.jw` - Modul pembantu yang dapat diimpor
- `demo_paket.sh` - Script demonstrasi sistem paket

## Cara Menjalankan Demonstrasi

1. Pastikan kompiler Jawa sudah dibangun
2. Jalankan script demonstrasi:

```bash
./demo_paket.sh
```

## Tentang Sistem Paket Jawa

Sistem paket Jawa memungkinkan pengembang untuk:

1. Membuat dan mendistribusikan paket kode yang dapat digunakan kembali
2. Mengimpor dan menggunakan paket pihak ketiga dalam proyek mereka
3. Mengelola dependensi proyek dengan mudah

Dokumentasi lebih lengkap tersedia di `/docs/PACKAGE_MANAGER.md`.

## Perintah Dasar

```bash
# Inisialisasi proyek baru
jawa paket init

# Pasang paket
jawa paket pasang nama-paket[@versi]

# Tampilkan paket terpasang
jawa paket daftar

# Hapus paket
jawa paket hapus nama-paket
```

## Penggunaan dalam Kode

```jawa
// Impor paket eksternal
impor "nama-paket" sebagai np;

// Impor modul dari proyek yang sama
impor "./modul.jw" sebagai mdl;

// Gunakan fungsi dari paket
np.fungsiDariPaket();
```
