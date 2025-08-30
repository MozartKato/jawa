# Cara Menggunakan Ekstensi Jawa Programming Language

## Instalasi

Untuk saat ini, ekstensi ini belum dipublikasikan ke VS Code Marketplace. Anda dapat menginstalnya secara lokal dengan langkah berikut:

1. Buka folder ekstensi:
   ```
   cd /home/yazid/project/jawa/extension
   ```

2. Instal dependensi (jika ada):
   ```
   npm install
   ```

3. Buat paket .vsix:
   ```
   vsce package
   ```
   (Jika Anda belum memiliki vsce, instal dengan `npm install -g vsce`)

4. Instal ekstensi dari file .vsix:
   - Buka VS Code
   - Tekan Ctrl+Shift+P
   - Ketik "Extensions: Install from VSIX"
   - Pilih file .vsix yang telah dibuat

## Penggunaan

Setelah diinstal, ekstensi akan otomatis aktif untuk file dengan ekstensi `.jw`.

### Syntax Highlighting

Ekstensi ini menyediakan syntax highlighting untuk:
- Kata kunci bahasa Jawa (`owahi`, `yen`, `liyane`, `ngulang`, dll.)
- Konstanta (`bener`, `salah`)
- Fungsi standar (`cithak`, `file_buka`, dll.)
- String dan angka
- Komentar

### Snippets

Ketikkan awalan berikut lalu tekan Tab untuk menggunakan snippet:
- `cithak`: Untuk mencetak ke konsol
- `yen`: Untuk pernyataan if
- `yen-liyane`: Untuk pernyataan if-else
- `ngulang`: Untuk loop for
- `owahi`: Untuk deklarasi variabel
- `file-waca`: Untuk membuka file untuk dibaca
- `file-tulis`: Untuk membuka file untuk ditulis
- `file-waca-kabeh`: Untuk membaca seluruh isi file
- `file-ada`: Untuk memeriksa apakah file ada

## Pengembangan

Untuk berkontribusi pada pengembangan ekstensi:

1. Clone repository:
   ```
   git clone https://github.com/MozartKato/jawa-lang.git
   ```

2. Buka folder ekstensi:
   ```
   cd jawa-lang/extension
   ```

3. Lakukan perubahan yang diperlukan

4. Test ekstensi dengan VS Code Extension Development Host:
   - Tekan F5 di VS Code
   - Buka file `.jw` di jendela VS Code yang baru

5. Submit pull request dengan perubahan Anda
