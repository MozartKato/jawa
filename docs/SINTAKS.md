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

## Definisi Fungsi

### Sintaks Fungsi
```
// Mendefinisikan fungsi dengan keyword 'gawe'
gawe nama_fungsi(parameter1: tipe1, parameter2: tipe2): tipe_return
{
    // Isi fungsi
    bali nilai_return  // Return menggunakan keyword 'bali'
}

// Contoh fungsi sederhana
gawe tambah(a: int, b: int): int
{
    bali a + b
}

// Contoh fungsi dengan string
gawe sapa(nama: string): string
{
    bali "Halo " + nama
}

// Contoh fungsi tanpa return value
gawe cetak_pesan(pesan: string): void
{
    cithak(pesan)
}
```

### Tipe Data untuk Parameter dan Return
```
int        // Bilangan bulat
double     // Bilangan desimal
string     // Teks/string
bool       // Boolean (bener/salah)
void       // Tidak ada return value
```

### Pemanggilan Fungsi
```
// Memanggil fungsi
owahi hasil = tambah(5, 10)
owahi ucapan = sapa("Budi")
cetak_pesan("Selamat datang")
```

## Pemrograman Berorientasi Objek (OOP)

### Definisi Kelas
```
// Mendefinisikan kelas dengan keyword 'bolo'
bolo NamaKelas
{
    // Properti kelas dengan keyword 'owahi'
    owahi nama_properti: tipe
    owahi properti_lain: tipe
    
    // Method/fungsi dalam kelas
    gawe nama_method(parameter: tipe): tipe_return
    {
        // Akses properti menggunakan 'this'
        this.nama_properti = nilai
        bali this.properti_lain
    }
}
```

### Contoh Kelas Lengkap
```
// Definisi kelas Person
bolo Person
{
    owahi name: string
    owahi age: int
    
    // Constructor
    gawe constructor(n: string, a: int): void
    {
        this.name = n
        this.age = a
    }
    
    // Method untuk mendapatkan info
    gawe info(): string
    {
        bali "Nama: " + this.name + ", Umur: " + this.age
    }
    
    // Method untuk mengubah nama
    gawe ubah_nama(nama_baru: string): void
    {
        this.name = nama_baru
    }
}
```

### Penggunaan Keyword `this`
```
// Di dalam method kelas, gunakan 'this' untuk akses properti
gawe set_nilai(nilai: int): void
{
    this.properti = nilai        // Set properti
    owahi temp = this.properti   // Get properti
}
```

### Pemanggilan Method Kelas
```
// Cara memanggil method kelas (manual)
Person person1;
person1.name = "Alice";
person1.age = 30;

// Memanggil method dengan format: KelasManajer_method(&objek)
cithak(Person_info(&person1))
```

## Keterbatasan

Berdasarkan pengujian, beberapa fitur bahasa yang tidak berhasil dijalankan:

1. **Constructor Otomatis**: Instansiasi objek dengan `Person("Alice", 30)` belum sepenuhnya berfungsi.
2. **Method Call Syntax**: Sintaks `objek.method()` belum didukung, harus menggunakan `Kelas_method(&objek)`.
3. **Tipe Data Kompleks**: Array dengan `array<tipe>` dan Map dengan sintaks `{}`.
4. **Try-Catch**: Blok `nyoba`, `cekel`, dan `pungkasan` tidak berfungsi.
5. **Statement `liyane`**: Sintaks `else` yang ditulis sebagai `liyane` tidak berfungsi dengan baik.

## Rekomendasi Coding

Untuk keberhasilan kode Jawa:

### Fungsi dan OOP
1. **Gunakan definisi fungsi**: Keyword `gawe` untuk mendefinisikan fungsi dengan type annotations
2. **Implementasi OOP**: Kelas dengan `bolo`, properti dengan `owahi`, dan method dengan `gawe`
3. **Gunakan `this`**: Untuk akses properti di dalam method kelas
4. **Return statement**: Gunakan `bali` untuk mengembalikan nilai dari fungsi

### Struktur Kode
1. Gunakan struktur data sederhana dengan pendekatan prosedural dan OOP
2. Pecah fungsi kompleks menjadi fungsi-fungsi kecil
3. Gunakan pemeriksaan kondisional untuk error handling
4. Pecah string yang panjang menjadi beberapa variabel terpisah
5. Gunakan variabel dan struktur kontrol dasar

### Contoh Kode Yang Berfungsi
```
// Fungsi sederhana
gawe hitung_luas(panjang: double, lebar: double): double
{
    bali panjang * lebar
}

// Kelas sederhana  
bolo Mobil
{
    owahi merk: string
    owahi tahun: int
    
    gawe info(): string
    {
        bali "Mobil " + this.merk + " tahun " + this.tahun
    }
}
```
