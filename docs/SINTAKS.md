# Dokumentasi Bahasa Pemrograman Jawa

Dokumen ini menjelaskan sintaks dan pola yang berhasil berfungsi pada bahasa pemrograman Jawa, berdasarkan hasil pengujian berbagai contoh.

## Sintaks Dasar

### Variabel dan Tipe Data
```
// Mendeklarasikan variabel
owahi nama_variabel = nilai

// Tipe data yang didukung
owahi string_variable = "teks"      // String
owahi integer_variable = 42         // Integer
owahi double_variable = 3.14        // Double/Float
owahi boolean_variable = bener      // Boolean (bener/salah)
```

### Output
```
// Mencetak ke layar
cithak("Teks")
cithak(variabel)
cithak("Teks " + variabel)  // Konkatenasi
```

### Struktur Kontrol
```
// Pernyataan If
menawa (kondisi)
{
    // Statements
}

// If tanpa Else (Else tidak didukung dengan sintaks liyane)
menawa (kondisi_1)
{
    // Statements 1
}
menawa (kondisi_2)
{
    // Statements 2
}

// Loop
ngulang (owahi i = 0; i < max; i++)
{
    // Statements
}
```

## Operasi String

### Metode String
```
// Memperoleh panjang string
owahi panjang = teks.length()      // Bahasa Inggris
owahi panjang = teks.dawane        // Bahasa Jawa

// Konversi huruf besar/kecil
owahi besar = teks.toUpperCase()   // Bahasa Inggris
owahi besar = teks.menyang_gedhe() // Bahasa Jawa
owahi kecil = teks.toLowerCase()   // Bahasa Inggris
owahi kecil = teks.menyang_cilik() // Bahasa Jawa

// Substring
owahi potongan = teks.substring(awal, akhir)  // Bahasa Inggris
owahi potongan = teks.potong(awal, akhir)     // Bahasa Jawa

// Pencarian dan penggantian
owahi hasil = teks.replace(cari, ganti)       // Bahasa Inggris
owahi hasil = teks.ganti(cari, ganti)         // Bahasa Jawa
```

### Operasi Array
```
// String split ke array
owahi array = teks.split(",")      // Bahasa Inggris
owahi array = teks.pisah(",")      // Bahasa Jawa

// Akses elemen array
owahi item = array[indeks]

// Mendapatkan panjang array
owahi panjang = array.length
```

## Operasi File

### Membuka dan Menutup File
```
// Membuka file
JawaFile* berkas = file_buka("nama_file.txt", "mode")
// Mode: "waca" (baca), "tulis" (tulis), "tambah" (append)

// Menutup file
file_tutup(berkas)
```

### Operasi Baca/Tulis
```
// Menulis ke file
file_tulis(berkas, "teks")
file_tulis_baris(berkas, "teks dengan newline")

// Membaca file
char* isi = file_waca_kabeh(berkas)    // Seluruh isi file
char* baris = file_waca_baris(berkas)  // Satu baris

// Cek akhir file
owahi eof = file_akhir(berkas)
```

### Operasi File Lainnya
```
// Mengecek keberadaan file
owahi ada = file_ada("nama_file.txt")

// Mendapatkan info file
FileInfo* info = file_info("nama_file.txt")

// Menghapus file
file_hapus("nama_file.txt")
```

## Penanganan Error

### Pemeriksaan Kondisional
```
// Pengecekan error
owahi error_terjadi = (kondisi_error)
menawa (error_terjadi)
{
    // Tangani error
}

// Contoh: Pemeriksaan file
JawaFile* berkas = file_buka("file.txt", "waca")
menawa (berkas == NULL)
{
    cithak("File tidak ditemukan")
}

// Contoh: Pengecekan pembagian dengan nol
menawa (pembagi == 0)
{
    cithak("Error: Pembagian dengan nol")
}
```

## Keterbatasan

Berdasarkan pengujian, beberapa fitur bahasa yang tidak berhasil dijalankan:

1. **Sintaks OOP**: Definisi kelas (`bolo`), pewarisan (`warisi`), constructor, penggunaan `this`.
2. **Tipe Data Kompleks**: Array dengan `array<tipe>` dan Map dengan sintaks `{}`.
3. **Try-Catch**: Blok `nyoba`, `cekel`, dan `pungkasan` tidak berfungsi.
4. **Statement `liyane`**: Sintaks `else` yang ditulis sebagai `liyane` tidak berfungsi dengan baik.

## Rekomendasi Coding

Untuk keberhasilan kode Jawa:
1. Gunakan struktur data sederhana dengan pendekatan prosedural
2. Hindari definisi fungsi kompleks
3. Gunakan pemeriksaan kondisional untuk error handling
4. Pecah string yang panjang menjadi beberapa variabel terpisah
5. Gunakan variabel dan struktur kontrol dasar
