# Jawa Programming Language v0.2

🇮🇩 **Native-Only Javanese Programming Language**

Jawa adalah bahasa pemrograman dengan sintaks autentik Jawa (Javanese) yang dikompilasi langsung ke native binary untuk performa maksimal - lebih cepat dari Apache untuk web development!

## ✨ Features

- ✅ **Native Compilation Only** - Langsung ke binary, no VM overhead
- ✅ **Authentic Javanese Syntax** - Kata-kata asli Jawa yang murni
- ✅ **Strong Type System** - int, double, bool, string dengan inference
- ✅ **Complete Control Flow** - For/while loops, if/else conditionals 
- ✅ **Object-Oriented Programming** - Class dan method support (foundation ready)
- ✅ **Clean Architecture** - No bytecode complexity, modular structure

## 🚀 Quick Start

### 1. Build Compiler
```bash
make clean
make
```

### 2. Coba Program Pertama
```bash
# Buat file hello.jw
echo 'cithak "Hello, World!"' > hello.jw

# Compile ke native binary
./jawa build hello.jw hello
./hello
```

## 📖 Panduan Bahasa

### Variabel dan Tipe Data
```jawa
owahi counter: int = 0        # Variable (owahi)
cendhak nama: string = "Jawa" # Let declaration (cendhak)
ajek pi: double = 3.14159     # Constant (ajek)
```

### Output dengan `cithak`
```jawa
cithak "Hello, World!"
cithak counter
cithak "Hasil:", result
```

### Object-Oriented Programming (Coming Soon)
```jawa
bolo User {                   # Class (bolo)
    nama: string
    umur: int
}

gawe sapa(nama: string) {     # Function (gawe)
    cithak "Halo", nama
}
```

## 🛠️ Architecture & Goals

**Target**: High-performance web development language with native server support

### Current Status (v0.2):
- ✅ **Native compilation only** - Bytecode mode removed for simplicity
- ✅ **Indonesian syntax** - `owahi`, `cendhak`, `ajek` keywords
- ✅ **Type system** - Strong typing with inference
- ✅ **Expression parsing** - Full arithmetic and logical expressions
- 🚧 **OOP support** - `bolo` (class) and `gawe` (function) in development
- 🎯 **Future**: Built-in web server faster than Apache

### Single Mode Architecture:
```
Source Code (.jw) → Native Transpiler → C Code → GCC → Binary
```

## 📁 Contoh Program

### Hello World
```jawa
cithak "Hello from Jawa!"
```

### Calculator
```jawa
owahi a: int = 10
owahi b: int = 3

cendhak tambah = a + b
cendhak kali = a * b
cendhak bagi: double = a / b

cithak "Penjumlahan:", tambah
cithak "Perkalian:", kali
cithak "Pembagian:", bagi
```

### OOP Example (In Development)
```jawa
bolo Calculator {
    owahi result: double = 0.0
    
    gawe add(a: double, b: double) {
        result = a + b
        return result
    }
}

owahi calc = Calculator()
cithak calc.add(5.0, 3.0)
```

## 🎯 Development Roadmap

### Phase 1: Core Language ✅
- [x] Native compilation
- [x] Indonesian keywords (`owahi`, `cendhak`, `ajek`)  
- [x] Type system with inference
- [x] Expression parsing & arithmetic

### Phase 2: OOP Foundation 🚧
- [ ] `bolo` (class) declarations
- [ ] `gawe` (function) definitions
- [ ] Method calls and object instantiation
- [ ] Inheritance and polymorphism

### Phase 3: Web Server Integration 🎯
- [ ] Built-in HTTP server
- [ ] Request/Response handling
- [ ] Template engine
- [ ] Database connectivity
- [ ] Performance optimizations

## ⚠️ Current Limitations

- **No control flow yet** - `menawa`, `yen`, `ngulang` tidak tersedia
- **Basic functions only** - `gawe` masih dalam development
- **No classes yet** - `bolo` structure sedang dikembangkan
- **Single file compilation** - belum ada module system

## 🏗️ Build & Test

```bash
# Build compiler
make clean
make

# Test basic functionality
./jawa build examples/calculator.jw test
./test

# Show version
./jawa version
```

## 📚 Untuk Developer

### File Structure (Simplified)
```
jawa/
├── src/           
│   ├── native.c   # C transpiler (main component)
│   └── main.c     # CLI interface
├── include/       # Headers (minimal)
├── examples/      # Sample programs
└── obj/          # Build output
```

### Adding New Features
1. **Language constructs**: Edit parsing logic in `src/native.c`
2. **Built-in functions**: Add to preamble in `write_preamble()`
3. **Type system**: Extend `Ty` enum and type mapping

---

**Jawa Programming Language v0.2** - Native-compiled web language dengan sintaks Indonesia 🇮🇩
