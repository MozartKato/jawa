# Jawa Programming Language v0.1

Jawa adalah bahasa pemrograman sederhana dengan sintaks berbahasa Indonesia yang dikembangkan untuk pembelajaran dan eksperimen. Bahasa ini mendukung dua mode kompilasi: bytecode dengan virtual machine dan transpilasi ke native C.

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

# Kompilasi dan jalankan (bytecode mode)
./jawa compile hello.jw hello.jbc
./jawa run hello.jbc

# Atau compile ke native binary
./jawa build hello.jw hello
./hello
```

## 📖 Panduan Bahasa

### Variabel dan Tipe Data

**Bytecode Mode (Simple Syntax):**
```jawa
counter = 0
nama = "Jawa"
result = 5 + 3
```

**Build Mode (Typed Syntax):**
```jawa
var counter: int = 0
let nama: string = "Jawa"
var result: double = 5.5 + 3.2
```

### Output dengan `cithak`
```jawa
cithak "Hello, World!"
cithak counter
cithak "Hasil:", result
```

### Control Flow

**While Loop (`menawa`):**
```jawa
counter = 0
menawa (counter < 3) {
    cithak "Iteration:", counter
    counter = counter + 1
}
```

**If-Else (`yen`/`liyane`):**
```jawa
yen (counter > 5) {
    cithak "Counter besar"
} liyane {
    cithak "Counter kecil"
}
```

## 🛠️ Mode Kompilasi

### 1. Bytecode Mode 
- **Sintaks**: Simple assignment (`x = 5`)
- **Pros**: Mudah digunakan, mendukung semua control flow
- **Cons**: Variable values tidak ditampilkan dalam output

```bash
./jawa compile program.jw program.jbc
./jawa run program.jbc
```

### 2. Build Mode (Native)
- **Sintaks**: Typed declarations (`var x: int = 5`)  
- **Pros**: Full C performance, variable values tampil dengan benar
- **Cons**: Belum support control flow structures

```bash
./jawa build program.jw program
./program
```

## 📁 Contoh Program

### Hello World (Works in both modes)
```jawa
cithak "Hello from Jawa!"
```

### Calculator (Build mode only)
```jawa
var a: int = 10
var b: int = 3

let tambah = a + b
let kali = a * b
let bagi: double = a / b

cithak "Penjumlahan:", tambah
cithak "Perkalian:", kali
cithak "Pembagian:", bagi
```

### Loop Example (Bytecode mode only)
```jawa
counter = 0
menawa (counter < 5) {
    cithak "Loop iteration"
    counter = counter + 1
}
cithak "Done!"
```

## ⚠️ Known Issues & Limitations

### Bytecode Mode Issues:
- ❌ **Variable values tidak muncul dalam output** - `cithak counter` hanya print nama variable, bukan nilainya
- ❌ **For loops (`ngulang`) infinite loop** - increment statement tidak ter-execute dengan benar
- ✅ While loops (`menawa`) bekerja dengan baik
- ✅ If-else (`yen`/`liyane`) bekerja dengan baik
- ✅ Arithmetic operations bekerja dengan benar

### Build Mode Issues:
- ❌ **Control flow belum diimplementasi** - tidak support `menawa`, `yen`, `ngulang`  
- ❌ **Hanya support simple assignments dan expressions**
- ✅ Variable declarations dengan tipe bekerja dengan baik
- ✅ Arithmetic expressions dan function calls bekerja dengan baik

### Syntax Incompatibility:
- **Bytecode mode**: `counter = 0` (simple assignment)
- **Build mode**: `var counter: int = 0` (typed declaration)
- Tidak bisa menggunakan file yang sama untuk kedua mode

## 🏗️ Architecture Overview

```
Source Code (.jw)
        |
        ├── Bytecode Path
        |   ├── compiler.c → .jbc file
        |   └── vm.c → execution
        |
        └── Build Path
            ├── native.c → .c file  
            ├── gcc → binary
            └── direct execution
```

### Core Components:
- **compiler.c**: Bytecode generator (supports loops, conditions)
- **vm.c**: Virtual machine interpreter  
- **native.c**: C transpiler (basic expressions only)
- **value.c**: Type system dan operations

## 🚧 Development Status

| Feature | Bytecode Mode | Build Mode |
|---------|---------------|------------|
| Variable Assignment | ✅ | ✅ |
| Variable Display | ❌ | ✅ |
| Arithmetic | ✅ | ✅ |
| String Output | ✅ | ✅ |
| While Loops | ✅ | ❌ |
| If-Else | ✅ | ❌ |
| For Loops | ❌ (infinite) | ❌ |
| Type System | Basic | Full |

## 🐛 Bug Reports

Jika menemukan bug atau ingin contribute:

1. **Variable Printing Bug** (Priority: High)
   - File: `src/vm.c`, `src/compiler.c` 
   - Issue: Variable values tidak muncul dalam bytecode mode

2. **For Loop Infinite Bug** (Priority: High)  
   - File: `src/compiler.c`
   - Issue: Increment statement dalam for loop tidak ter-execute

3. **Control Flow di Build Mode** (Priority: Medium)
   - File: `src/native.c`
   - Issue: Belum ada implementasi loops dan conditionals

## 📚 Untuk Developer

### Build System
```bash
make clean    # Hapus build artifacts
make          # Build compiler
make install  # (belum tersedia)
```

### Testing
```bash
# Test bytecode mode
./jawa compile examples/hello.jw test.jbc
./jawa run test.jbc

# Test build mode  
./jawa build examples/calculator.jw test-native
./test-native
```

### File Structure
```
jawa/
├── src/           # Source code
│   ├── compiler.c # Bytecode compiler
│   ├── vm.c      # Virtual machine  
│   ├── native.c  # C transpiler
│   └── main.c    # CLI interface
├── include/       # Headers
├── examples/      # Sample programs
└── obj/          # Build output
```

---

**Jawa Programming Language** - Eksperimental educational language dengan focus pada kesederhanaan dan pembelajaran konsep compiler/interpreter.
