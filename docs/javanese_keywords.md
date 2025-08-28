# Update: Keyword Bahasa Jawa Authentic

## ✅ Keyword Baru yang Telah Diimplementasi

### **Deklarasi Variabel**
- `const` → `ajek` (dari "ajeg" = tetap/konstan)
- `var` → `owahi` (dari "owah" = ubah/ganti) 
- `let` → `cendhak` (dari "cendhak" = pendek/sementara)

### **Kontrol Alur** 
- `if` → `yen` (dari "yen" = jika) ✅ **WORKING**
- `else` → `liyane` (dari "liyane" = yang lain) ⚠️ **IN PROGRESS**

## 🎯 **Status Implementasi**

| Fitur | Bytecode | Native | Status |
|-------|----------|---------|---------|
| `owahi` declarations | ✅ | ✅ | **COMPLETE** |
| `cendhak` declarations | ✅ | ✅ | **COMPLETE** |
| `ajek` declarations | ✅ | ✅ | **COMPLETE** |
| `yen` simple if | ⚠️ | ✅ | **PARTIAL** |
| `liyane` else | ❌ | ⚠️ | **TODO** |

## 📝 **Contoh Penggunaan**

### Basic Declarations
```jawa
owahi angka: int = 42
cendhak nama: string = "Jawa Modern"  
ajek pi: double = 3.14159
owahi aktif: bool = true

cithak nama
cithak angka
cithak pi
cithak aktif
```

### Simple Conditionals
```jawa
owahi umur: int = 20

yen umur >= 18 {
    cithak "Dewasa"
}

yen umur < 18 {
    cithak "Bocah"
}
```

## 🚀 **Cara Test**

```bash
# Test keyword baru
./jawa compile examples/modern_jawa.jw test.jbc
./jawa run test.jbc

# Test native compilation
./jawa native examples/modern_jawa.jw test-native
./test-native

# Test simple if
./jawa native examples/simple_if.jw test-if  
./test-if
```

## 🎭 **Filosofi Keyword**

**Mengapa Bahasa Jawa?**
- `owahi` = "mengubah" - merefleksikan nature mutable variable
- `cendhak` = "pendek/sementara" - untuk variable scope terbatas  
- `ajek` = "tetap/konstan" - untuk nilai yang tidak berubah
- `yen` = "jika" - conditional logic yang natural
- `liyane` = "yang lain" - alternative path yang jelas

Keyword ini membuat bahasa programming lebih familiar dan accessible untuk penutur bahasa Jawa! 🇮🇩

## 🔧 **Next Steps**

1. **Complete bytecode support** untuk `yen` dengan jump instructions
2. **Fix `liyane` parsing** di native transpiler  
3. **Add nested if support** dan complex conditionals
4. **Implement loops**: `nalika` (while), `kanggo` (for)
5. **Add functions**: `fungsi` atau `gawean`

---

**Status: Keyword authentic bahasa Jawa berhasil diimplementasikan! 🎉**
