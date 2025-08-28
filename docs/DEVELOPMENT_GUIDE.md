# Jawa Programming Language - Development Guide

## Project Structure (Clean & Organized)

```
jawa/
├── 📄 .gitignore           # Git ignore rules
├── 📄 README.md            # Main documentation
├── 📄 Makefile            # Build system
├── 🔧 jawa                # Compiled jawa compiler
├── 📁 src/                # Core source code
│   ├── main.c             # Application entry
│   ├── lexer.c            # Tokenization & Javanese support
│   ├── parser.c           # Syntax parsing & dual-language mapping
│   ├── compiler.c         # C code generation
│   ├── native.c           # Native functions library
│   ├── vm.c               # Virtual machine (future)
│   ├── value.c            # Value operations
│   ├── stack.c            # Stack management
│   ├── gc.c               # Garbage collection
│   └── vartable.c         # Variable table
├── 📁 include/            # Header files
├── 📁 examples/           # Production-ready examples
│   ├── working_features.jw    # Comprehensive feature demo
│   ├── quick_demo.jw          # Quick showcase
│   ├── language_comparison.jw # English vs Javanese syntax
│   ├── string_operations.jw   # String methods demo
│   ├── localization_test.jw   # Localization validation
│   └── archived/              # Historical test files
├── 📁 docs/               # Documentation
│   ├── PROJECT_COMPLETION_SUMMARY.md
│   ├── compilation_strategies.md
│   └── javanese_keywords.md
├── 📁 build/              # Compiled binaries and test executables
└── 📁 obj/               # Object files (generated)
```

## Development Workflow

### Building the Project
```bash
# Clean build
make clean
make

# Test core functionality
./jawa build examples/working_features.jw demo
./demo
```

### Adding New Features
1. Update appropriate source files in `src/`
2. Add corresponding headers in `include/` if needed
3. Create test example in `examples/`
4. Update documentation in `README.md`
5. Test thoroughly

### Code Organization Rules
- **Source Code**: Only in `src/` directory
- **Headers**: Only in `include/` directory  
- **Examples**: Production examples in `examples/`, tests in `examples/archived/`
- **Binaries**: Auto-generated in `build/` and `obj/`
- **Documentation**: In `docs/` and main `README.md`

## Maintenance Commands

### Clean Build Environment
```bash
# Remove all generated files
make clean

# Rebuild from scratch
make

# Clean examples build artifacts
rm -f examples/*.c build/*
```

### Test Suite
```bash
# Test all working examples
./jawa build examples/working_features.jw test && ./test
./jawa build examples/quick_demo.jw test && ./test
./jawa build examples/language_comparison.jw test && ./test
```

### Development Tips
- Keep production examples in main `examples/` folder
- Archive experimental code in `examples/archived/`
- Use descriptive commit messages
- Test both English and Javanese syntax
- Maintain backward compatibility

## Project Status: Production Ready ✅
- Core features: 100% working
- Dual language support: Complete
- Documentation: Comprehensive  
- Examples: Tested and verified
- Code organization: Clean and maintainable
