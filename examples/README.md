# Jawa Programming Language Examples

This directory contains various code examples demonstrating the features of the Jawa programming language. All examples have been **tested and verified to work without errors**.

## 📁 Organized Structure

Examples are now organized into categorized folders for better learning progression:

### [1-basics/](./1-basics/) - Fundamental Examples
- **simple.jw** - Basic "Hello World" and arithmetic operations  
- **quick_demo.jw** - Comprehensive language feature demonstration  
- **data_basic.jw** - Basic data manipulation  
- **input_demo.jw** - Input/output operations  
- **best_practices.jw** - Coding best practices  
- **working_features.jw** - Complete feature overview  
- **simple_test.jw** - Basic syntax testing  

### [2-functions/](./2-functions/) - Function Examples  
- **fungsi_demo.jw** - Function definitions with `gawe` keyword  
- **tes_fungsi.jw** - Function testing and validation  

### [3-oop/](./3-oop/) - Object-Oriented Programming  
- **oop_demo_fix.jw** - Complete OOP with inheritance and `this` keyword  
- **oop_demo.jw** - Basic OOP demonstration  
- **oop_enhanced_demo.jw** - Advanced OOP patterns  

### [4-strings/](./4-strings/) - String Operations  
- **string_operations.jw** - Comprehensive string manipulation (English + Javanese syntax)  
- **string_enhanced_demo.jw** - Advanced string features  

### [5-file-operations/](./5-file-operations/) - File I/O  
- **file_minimal.jw** - Basic file operations  
- **file_operations.jw** - Complete file manipulation  
- **file_operations_fix.jw** - Robust file operations with error handling  
- **file_operations_simple.jw** - Simple file read/write  

### [6-error-handling/](./6-error-handling/) - Error Management  
- **error_simple.jw** - Basic error handling patterns  
- **error_handling.jw** - Comprehensive error scenarios  
- **error_handling_fix.jw** - Improved error handling  
- **error_handling_fix2.jw** - Advanced error management  

### [7-advanced/](./7-advanced/) - Advanced Features  
- **language_comparison.jw** - English vs Javanese syntax comparison  
- **data_structures.jw** - Complex data structures  
- **data_structure_fix.jw** - Optimized data operations  
- **localization_test.jw** - Multi-language support  

## 🚀 How to  Examples

To  any example, use:

```bash
cd /path/to/jawa
./jawa  examples/[category]/[filename].jw
```

Examples:
```bash
# Basic examples
./jawa  examples/1-basics/simple.jw
./jawa  examples/1-basics/quick_demo.jw

# Function examples  
./jawa  examples/2-functions/fungsi_demo.jw

# OOP examples
./jawa  examples/3-oop/oop_demo_fix.jw

# String operations
./jawa  examples/4-strings/string_operations.jw

# Advanced features
./jawa  examples/7-advanced/language_comparison.jw
```

## ✅ Verified Status

**ALL EXAMPLES TESTED AND WORKING** - No compilation or time errors!
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

kelas Mobil tuan Kendaraan {
    owahi jumlahRoda = 4
}
```

### Operasi File

```jawa
owahi berkas = file_buka("data.txt", "waca")
owahi isi = berkas.waca_kabeh()
berkas.tutup()
```
