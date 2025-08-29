# Contoh Program Bahasa Jawa

Direktori ini berisi berbagai contoh kode dalam Bahasa Jawa untuk mendemonstrasikan fitur-fitur bahasa.

## Cara Menjalankan Contoh

Untuk menjalankan contoh, gunakan perintah:

```bash
./jawa build examples/nama_file.jw nama_output
./nama_output
```

Misalnya:

```bash
./jawa build examples/string_enhanced_demo.jw string_demo
./string_demo
```

## Daftar Contoh

### Fitur Dasar

- `simple.jw` - Contoh dasar "Hello World" dan sintaks dasar
- `tes_fungsi.jw` - Demonstrasi fungsi dan parameter
- `quick_demo.jw` - Demo singkat berbagai fitur dasar

### Struktur Data dan Kontrol Alur

- `data_structures.jw` - Array, map, dan struktur data lainnya
- `error_handling.jw` - Penanganan kesalahan dengan try-catch

### Fitur String

- `string_operations.jw` - Operasi string dasar
- `string_enhanced_demo.jw` - Demonstrasi fitur string yang ditingkatkan

### Operasi File

- `file_operations.jw` - Operasi file dasar (baca/tulis)

### OOP

- `oop_demo.jw` - Dasar-dasar Object-Oriented Programming
- `oop_enhanced_demo.jw` - Fitur OOP lanjutan (inheritance, polymorphism)

### Lainnya

- `working_features.jw` - Fitur yang sudah terimplementasi dan berfungsi
- `language_comparison.jw` - Perbandingan dengan bahasa pemrograman lain
- `localization_test.jw` - Penggunaan karakter khusus dan lokalisasi
- `input_demo.jw` - Interaksi dengan input pengguna

## Ringkasan Syntax

### Variabel

```jawa
owahi nama = "Joko"
cendhak umur = 25
ajek PI = 3.14
```

### Fungsi

```jawa
gawe tambah(a, b) {
    bali a + b
}
```

### Kontrol Alur

```jawa
yen (kondisi) {
    // kode
} liyane yen (kondisi2) {
    // kode
} liyane {
    // kode
}

untuk (owahi i = 0; i < 10; i++) {
    // kode
}

untuk (owahi item ing daftar) {
    // kode
}
```

### OOP

```jawa
kelas Kendaraan {
    private owahi merek = ""
    
    gawe konstruktor(m) {
        self.merek = m
    }
    
    gawe getMerek() {
        bali self.merek
    }
}

kelas Mobil turunan Kendaraan {
    owahi jumlahRoda = 4
}
```

### Operasi File

```jawa
owahi berkas = file_buka("data.txt", "waca")
owahi isi = berkas.waca_kabeh()
berkas.tutup()
```
