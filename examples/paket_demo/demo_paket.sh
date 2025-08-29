#!/bin/bash
# Script demonstrasi package manager Jawa

# Direktori saat ini
CURRENT_DIR=$(pwd)
echo "===== Demonstrasi Package Manager Jawa ====="

# 1. Membuat proyek baru
echo -e "\n1. Membuat proyek baru"
mkdir -p /tmp/jawa-demo
cd /tmp/jawa-demo
echo "   Direktori proyek: $(pwd)"

# 2. Menginisialisasi proyek
echo -e "\n2. Menginisialisasi proyek dengan 'jawa paket init'"
$CURRENT_DIR/jawa paket init
echo "   File yang dibuat:"
ls -la

# 3. Menampilkan daftar paket
echo -e "\n3. Menampilkan daftar paket terpasang"
$CURRENT_DIR/jawa paket daftar

# 4. Memasang paket (simulasi)
echo -e "\n4. Memasang paket (simulasi)"
echo "   Catatan: Pemasangan paket aktual membutuhkan repositori paket yang aktif"
echo "   Perintah: jawa paket pasang matematika@1.0.0"

# 5. Menggunakan paket dalam kode
echo -e "\n5. Membuat file yang menggunakan paket"
cat > src/contoh_paket.jw << 'EOF'
// Contoh penggunaan paket

// Impor modul lokal
impor "./utama.jw" sebagai app;

// Impor paket eksternal (akan gagal karena belum diinstal)
coba {
    impor "matematika" sebagai mtk;
    cetak("2 + 2 = " + mtk.tambah(2, 2));
} kecuali(e) {
    cetak("Paket matematika tidak ditemukan: " + e);
}

// Panggil fungsi dari modul lokal
app.utama();

cetak("Demonstrasi sistem paket selesai!");
EOF

echo "   File contoh_paket.jw berhasil dibuat"
cat src/contoh_paket.jw

# 6. Membersihkan
echo -e "\n6. Membersihkan"
cd $CURRENT_DIR
echo "   Kembali ke direktori awal: $(pwd)"
# Uncomment untuk membersihkan direktori demo
# echo "   Menghapus direktori demo"
# rm -rf /tmp/jawa-demo

echo -e "\n===== Demonstrasi Selesai ====="
