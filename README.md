# 🌟 Jawa Programming Language v0.3

**Menyatukan tradisi Jawa dengan pemrograman modern** • *Uniting Javanese tradition with modern programming*

![Jawa Programming](https://img.shields.io/badge/Language-Jawa-green)
![Version](https://img.shields.io/badge/Version-0.3-blue)
![Status](https://img.shields.io/badge/Status-Production%20Ready-success)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)

## 🌟 Features

### ✅ **Core Language Features**
- **Variable Declaration**: `owahi` (var/let) with type inference ✅
- **Print Statements**: `cithak` / `tulis` (print/output) ✅
- **Basic Loops**: `ngulang` (for) ✅  
- **Functions**: `gawe` (function) with `bali` (return) ✅
- **Object-Oriented Programming**: `bolo` (class) with inheritance ✅
- **Boolean Values**: `bener`/`salah` (true/false) ✅
- **String Operations**: Comprehensive string manipulation ✅

### ✅ **String Operations (Dual Syntax)**
| English Syntax | Javanese Syntax | Status | Function |
|---------------|----------------|---------|-----------|
| `.toUpperCase()` | `.menyang_gedhe()` | ✅ | Convert to uppercase |
| `.toLowerCase()` | `.menyang_cilik()` | ✅ | Convert to lowercase |
| `.replace(a,b)` | `.ganti(a,b)` | ✅ | Replace substring |
| `.substring(0,5)` | `.potong(0,5)` | ✅ | Extract substring |
| `.length` | `.dawane` | ✅ | String length |
| `.split(",")` | `.pisah(",")` | ⚠️ | Split string (needs fix) |

### ✅ **Working Features**
- **Basic Programs**: Hello world, variables, print statements
- **String Manipulation**: Case conversion, substring, replace  
- **Functions**: Custom functions with parameters and return values
- **Object-Oriented**: Classes with properties and methods
- **Loops**: Basic for loops without complex expressions
- **Clean Compilation**: No debug output, production ready
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
./jawa mlayu examples/1-basics/simple.jw

# Or compile and run separately  
./jawa build examples/1-basics/simple.jw hello_program
./hello_program
```

## 🚧 Roadmap

### ✅ Completed (v0.3)
- [x] Basic syntax and keywords
- [x] String operations (dual language)
- [x] Basic loop constructs (`ngulang`)
- [x] Function definitions (`gawe` functions)
- [x] Object-oriented features (`bolo` classes)
- [x] Method chaining (basic)
- [x] Production-ready compilation
- [x] Organized examples structure

### 🔄 In Progress (v0.4)
- [ ] Array operations (split/pisah function fixing)
- [ ] Control flow statements (if/else conditions)
- [ ] Complex string concatenation with integers
- [ ] While loops (`menawa`)
- [ ] Error handling improvements

### 📋 Planned (v1.0)
- [ ] Constants (`ajek`) 
- [ ] Error handling (`nyoba`/`cekel`)
- [ ] File I/O operations
- [ ] Module system (`impor`/`ekspor`)
- [ ] Package manager
- [ ] Standard library
- [ ] IDE support

### Hello World
```jawa
# Basic Hello World
cithak("Sugeng Rawuh ing Jawa Programming!")

# With variables
owahi nama = "Dunia"
cithak("Halo " + nama)
```

### String Operations (Working)
```jawa
# English syntax
owahi text = "Hello World"
owahi upper = text.toUpperCase()
cithak("Result: " + upper)

# Javanese syntax  
owahi teks = "Sugeng Enjing"
owahi gedhe = teks.menyang_gedhe()
cithak("Hasil: " + gedhe)

# Replace operations
owahi hasil = text.ganti("World", "Jawa")
cithak("Changed: " + hasil)
```

### Basic Loops
```jawa
# Simple for loop
ngulang (owahi i = 0; i < 5; i++) {
    cithak("Iteration")
}
```

## 📚 Working Examples

### Basic Program
```jawa
# File: basic_demo.jw
cithak("=== Jawa Programming Demo ===")

owahi nama = "Sari"
owahi status = "Programming"

cithak("Nama: " + nama)
cithak("Status: " + status)
```

### String Manipulation
```jawa
# File: string_demo.jw
owahi kalimat = "Jawa Programming Language"

# Length
cithak("Length: " + int_to_string(kalimat.dawane))

# Case conversion
cithak("Upper: " + kalimat.toUpperCase())
cithak("Lower: " + kalimat.menyang_cilik())

# Replace
owahi replaced = kalimat.ganti("Programming", "Modern")
cithak("Replaced: " + replaced)
```

### Function Examples
```jawa
# File: function_demo.jw
gawe int tambah(int a, int b) {
    bali a + b;
}

owahi hasil = tambah(10, 5)
cithak("Result: " + int_to_string(hasil))
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
# Run Jawa source directly
./jawa mlayu input_file.jw

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
│   └── oop/               # OOP implementation
├── include/               # Header files
├── examples/              # Organized example programs
│   ├── 1-basics/          # Basic language features
│   ├── 2-functions/       # Function examples
│   ├── 3-oop/             # Object-oriented examples
│   ├── 4-strings/         # String manipulation
│   ├── 5-file-operations/ # File I/O examples
│   ├── 6-error-handling/  # Error handling patterns
│   └── 7-advanced/        # Advanced features
├── extension/             # VS Code language extension
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