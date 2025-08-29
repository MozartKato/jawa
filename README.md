# 🌟 Jawa Programming Language v0.3

**Menyatukan tradisi Jawa dengan pemrograman modern** • *Uniting Javanese tradition with modern programming*

![Jawa Programming](https://img.shields.io/badge/Language-Jawa-green)
![Version](https://img.shields.io/badge/Version-0.3-blue)
![Status](https://img.shields.io/badge/Status-Production%20Ready-success)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)

## 🌟 Features

### ✅ **Core Language Features**
- **Variable Declaration**: `owahi` (var/let) with type inference
- **Constants**: `ajek` (const) for immutable values  
- **Print Statements**: `cithak` (print/output)
- **Conditional Flow**: `yen`/`liyane`/`liyane yen` (if/else/else if)
- **Loops**: `ngulang` (for), `menawa` (while)
- **Functions**: `gawe` (function) with `bali` (return)
- **Error Handling**: `nyoba`/`cekel`/`pungkasan` (try/catch/finally)
- **Boolean Values**: `bener`/`salah` (true/false)
- **File Input/Output**: File operations and management
- **Object-Oriented Programming**: Class definition, inheritance
- **Package Management**: Dependency management system

### ✅ **String Operations (Dual Syntax)**
| English Syntax | Javanese Syntax | Function |
|---------------|----------------|-----------|
| `.toUpperCase()` | `.menyang_gedhe()` | Convert to uppercase |
| `.toLowerCase()` | `.menyang_cilik()` | Convert to lowercase |
| `.split(",")` | `.pisah(",")` | Split string by delimiter |
| `.replace(a,b)` | `.ganti(a,b)` | Replace substring |
| `.substring(0,5)` | `.potong(0,5)` | Extract substring |
| `.length` / `.length()` | `.dawane` | String length |
| `concat(a,b)` | `gabung(a,b)` | Concatenate strings |
| `.trim()` | `.resiki()` | Remove whitespace |
| `.contains("text")` | `.ngandhut("text")` | Check substring existence |
| `.toCharArray()` | `.ke_karakter()` | Convert to character array |

### ✅ **Array Operations**
- **Creation**: Via string splitting (`split()` / `pisah()`)
- **Indexing**: `array[index]` syntax
- **Length Property**: `.length` / `.dawane`
- **Loop Integration**: Full support in `ngulang` loops

### ✅ **Advanced Features**
- **Method Chaining**: `text.replace("a","b").toUpperCase().substring(0,10)`
- **Auto Type Conversion**: Automatic int/bool to string in concatenation
- **String Concatenation**: `"text" + variable + "more"`
- **Complex Expressions**: Nested operations with proper precedence
- **Cross-Platform Support**: Windows, macOS, and Linux compatibility
- **Package Management**: Import and manage third-party libraries

## 🚀 Quick Start

### Installation
```bash
# Clone the repository
git clone https://github.com/MozartKato/jawa
cd jawa

# Build the compiler (cross-platform)
make

# Run an example
./jawa run examples/quick_demo.jw

# Or compile and run separately
./jawa build examples/quick_demo.jw demo
./demo
```

### Package Management
```bash
# Initialize a new project
jawa paket init

# Install a package
jawa paket pasang matematika@1.0.0

# List installed packages
jawa paket daftar

# Use packages in code
impor "matematika" sebagai mtk
```

### Hello World
```jawa
# Basic Hello World
cithak("Sugeng Rawuh ing Jawa Programming!")

# With variables
owahi nama = "Dunia"
cithak("Halo " + nama)
```

### String Operations
```jawa
# English syntax
owahi text = "Hello World"
owahi upper = text.toUpperCase()
owahi words = text.split(" ")

# Javanese syntax  
owahi teks = "Sugeng Enjing"
owahi gedhe = teks.menyang_gedhe()
owahi tembung = teks.pisah(" ")

# Mixed usage
owahi hasil = text.ganti("World", "Jawa").menyang_gedhe()
cithak(hasil)  # Output: HELLO JAWA
```

### Arrays and Loops
```jawa
# Array operations
owahi data = "apel,jeruk,mangga".pisah(",")
owahi jumlah = data.dawane

# Loop through array
ngulang (owahi i = 0; i < jumlah; i++) {
    cithak("Buah ke-" + i + ": " + data[i])
}

# While loop
owahi counter = 0
menawa (counter < 3) {
    cithak("Counter: " + counter)
    counter = counter + 1
}
```

### Control Flow
```jawa
owahi nilai = 85

yen (nilai >= 90) {
    cithak("Grade: A")
} liyane yen (nilai >= 80) {
    cithak("Grade: B")  
} liyane {
    cithak("Grade: C")
}

# Boolean operations
owahi aktif = bener
owahi premium = salah

yen (aktif && !premium) {
    cithak("Basic user")
}
```

## 📚 Language Guide

### Variables and Types
```jawa
# Type inference (recommended)
owahi nama = "Jawa"           # string
owahi umur = 25               # int
owahi tinggi = 175.5          # double
owahi aktif = bener           # bool

# Explicit typing (optional)
owahi kota: string = "Yogyakarta"
owahi populasi: int = 400000

# Constants
ajek PI = 3.14159
ajek APP_NAME = "Jawa Programming"
```

### String Manipulation
```jawa
owahi kalimat = "Jawa Programming Language"

# Length
cithak(kalimat.length())      # Method call: 25
cithak(kalimat.dawane)        # Property access: 25

# Case conversion
cithak(kalimat.toUpperCase())     # JAWA PROGRAMMING LANGUAGE
cithak(kalimat.menyang_cilik())   # jawa programming language

# Substring
cithak(kalimat.substring(0, 4))   # Jawa
cithak(kalimat.potong(5, 16))     # Programming

# Replace
cithak(kalimat.replace("Programming", "Bahasa"))  # Jawa Bahasa Language
cithak(kalimat.ganti("Language", "Modern"))       # Jawa Programming Modern

# Splitting
owahi kata = kalimat.split(" ")    # ["Jawa", "Programming", "Language"]
owahi bagian = kalimat.pisah(" ")  # Same result with Javanese syntax

# Method chaining
owahi result = kalimat.ganti("Programming", "Modern")
                     .menyang_gedhe()
                     .potong(0, 10)
cithak(result)  # JAWA MODER
```

### Array Operations
```jawa
# Array creation
owahi colors = "red,green,blue".split(",")
owahi warna = "abang,ijo,biru".pisah(",")

# Array access
cithak(colors[0])              # red
cithak(warna[warna.dawane-1])  # biru

# Array length
cithak(colors.length)          # 3
cithak(warna.dawane)           # 3

# Array processing
ngulang (owahi i = 0; i < colors.dawane; i++) {
    cithak("Color " + i + ": " + colors[i])
}
```

### Loop Constructs
```jawa
# For loop variations
ngulang (owahi i = 0; i < 10; i++) {          # Standard increment
    cithak("Count: " + i)
}

ngulang (owahi j = 0; j < 10; j = j + 2) {    # Custom increment
    cithak("Even: " + j)
}

# While loop
owahi x = 0
menawa (x < 5) {
    cithak("Value: " + x)
    x = x + 1
}
```

### Type Conversion
```jawa
# Automatic conversion in string concatenation
owahi angka = 42
owahi desimal = 3.14
owahi boolean = bener

cithak("Number: " + angka)     # Number: 42
cithak("Pi: " + desimal)       # Pi: 3.14
cithak("Status: " + boolean)   # Status: true

# Complex expressions
owahi x = 10
owahi y = 5
cithak("Result: " + (x + y))   # Result: 15 (auto-converted)
```

## 🎯 Examples

### Basic Program
```jawa
# File: hello.jw
cithak("=== Jawa Programming Demo ===")

owahi nama = "Sari"
owahi umur = 25
owahi kota = "Yogyakarta"

cithak("Nama: " + nama)
cithak("Umur: " + umur + " tahun")
cithak("Kota: " + kota)

yen (umur >= 17) {
    cithak("Status: Dewasa")
} liyane {
    cithak("Status: Anak-anak")
}
```

### String Processing
```jawa
# File: text_processor.jw
owahi input = "  Jawa Programming Language  "

# Clean and process
owahi clean = input.ganti(" ", "_")
                   .menyang_cilik()
                   .potong(2, input.dawane - 2)

cithak("Original: '" + input + "'")
cithak("Processed: '" + clean + "'")

# Split and analyze
owahi words = input.split(" ")
cithak("Word count: " + words.dawane)

ngulang (owahi i = 0; i < words.length; i++) {
    yen (words[i].length > 0) {
        cithak("Word " + i + ": " + words[i])
    }
}
```

### Data Processing
```jawa
# File: csv_processor.jw
owahi csv_data = "name,age,city|John,25,NYC|Jane,30,LA"
owahi rows = csv_data.pisah("|")
owahi headers = rows[0].split(",")

cithak("CSV Processing:")
cithak("Headers: " + headers.length + " columns")

ngulang (owahi row = 1; row < rows.dawane; row++) {
    owahi data = rows[row].split(",")
    cithak("Record " + row + ":")
    
    ngulang (owahi col = 0; col < headers.length; col++) {
        cithak("  " + headers[col] + ": " + data[col])
    }
}
```

## 🔧 Compilation and Usage

### Build System
```bash
# Build the compiler
make

# Clean build files
make clean

# Build and run example
./jawa build examples/complete_demo.jw demo
./demo
```

### Command Line Usage
```bash
# Compile Jawa source to executable
./jawa build input_file.jw output_name

# Show version
./jawa version

# Help
./jawa
```

## 📁 Project Structure

```
jawa/
├── src/                    # Source code
│   ├── main.c             # Main compiler entry
│   ├── lexer.c            # Lexical analyzer
│   ├── parser.c           # Expression parser  
│   ├── statements.c       # Statement parser
│   ├── native.c           # C code generation
│   ├── string_ops.c       # String operations
│   └── oop/               # OOP features (future)
├── include/               # Header files
├── examples/              # Example programs
│   ├── complete_demo.jw   # Full feature demonstration
│   ├── string_operations.jw # String handling examples
│   ├── arrays_loops.jw    # Array and loop examples
│   └── control_flow.jw    # Conditional statements
├── Makefile              # Build configuration
└── README.md             # This file
```

## 🌐 Language Philosophy

Jawa Programming Language is designed to:

1. **Honor Javanese Culture**: Incorporate traditional Javanese terms and concepts
2. **Embrace Modern Programming**: Support contemporary programming paradigms
3. **Provide Flexibility**: Allow both English and Javanese syntax
4. **Enable Learning**: Make programming accessible to Javanese speakers
5. **Bridge Tradition and Innovation**: Connect cultural heritage with technology

### Keyword Mapping

| Concept | English | Javanese | Meaning |
|---------|---------|----------|---------|
| Variable | var/let | owahi | "change/transform" |
| Constant | const | ajek | "fixed/permanent" |
| Print | print | cithak | "print/publish" |
| If | if | yen | "if/when" |
| Else | else | liyane | "other/different" |
| While | while | menawa | "when/while" |
| For | for | ngulang | "repeat/iterate" |
| True | true | bener | "correct/true" |
| False | false | salah | "wrong/false" |
| Function | function | gawe | "make/create" |
| Class | class | bolo | "group/category" |

## 🚧 Roadmap

### ✅ Completed (v0.25)
- [x] Basic syntax and keywords
- [x] String operations (dual language)
- [x] Array operations
- [x] Control flow statements
- [x] Loop constructs  
- [x] Method chaining
- [x] Type conversion
- [x] Boolean operations

### 🔄 In Progress
- [ ] Function definitions (`gawe` functions)
- [ ] Object-oriented features (`bolo` classes)
- [ ] Error handling (`nyoba`/`cekel`)
- [ ] Module system (`impor`/`ekspor`)

### 📋 Planned (v0.3)
- [ ] File I/O operations
- [ ] Network operations
- [ ] Database integration
- [ ] Package manager
- [ ] Standard library
- [ ] IDE support

## 🤝 Contributing

We welcome contributions to Jawa Programming Language!

### Areas for Contribution
- Language feature implementation
- Standard library functions
- Documentation and examples
- IDE plugins and tools
- Community building

### Development Setup
```bash
git clone https://github.com/MozartKato/jawa
cd jawa
make
# Start coding!
```

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- Inspired by the rich Javanese language and culture
- Built with modern C programming practices
- Community-driven development approach

---

**Jawa Programming Language** - *Bridging tradition with innovation* 🌟