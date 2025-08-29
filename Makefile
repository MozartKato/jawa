# Deteksi OS secara otomatis
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    CC := gcc
    BIN_EXT := .exe
    SHARED_EXT := .dll
    RM_CMD := del /Q
    RM_DIR_CMD := rmdir /S /Q
    MKDIR_CMD := mkdir
            # Windows-specific flags
        CFLAGS = -Wall -O2 -Iinclude -Wno-format-truncation -D_WIN32
        # Tambahkan library Windows jika diperlukan
        LDFLAGS = -lm -lcurl -ljson-c
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        DETECTED_OS := Linux
        CC := gcc
        BIN_EXT :=
        SHARED_EXT := .so
        RM_CMD := rm -f
        RM_DIR_CMD := rm -rf
        MKDIR_CMD := mkdir -p
        # Linux-specific flags
        CFLAGS = -Wall -O2 -Iinclude -Wno-format-truncation -D__linux__
        LDFLAGS = -lm -lcurl -ljson-c
    endif
    ifeq ($(UNAME_S),Darwin)
        DETECTED_OS := macOS
        CC := clang
        BIN_EXT :=
        SHARED_EXT := .dylib
        RM_CMD := rm -f
        RM_DIR_CMD := rm -rf
        MKDIR_CMD := mkdir -p
        # macOS-specific flags
        CFLAGS = -Wall -O2 -Iinclude -Wno-format-truncation -D__APPLE__
        LDFLAGS = -lm -lcurl -ljson-c
    endif
endif

# Output hasil deteksi
$(info Building for $(DETECTED_OS) platform)

SRC = src/native.c src/main.c src/lexer.c src/parser.c src/statements.c \
      src/string_ops.c src/string_enhanced.c src/file_ops.c \
      src/oop/class.c src/oop/class_enhanced.c src/package_manager.c
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))

.PHONY: all clean test examples install uninstall

all: jawa$(BIN_EXT)

jawa$(BIN_EXT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Build object files (create nested directories)
obj/%.o: src/%.c | obj obj/oop
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directories
obj:
	$(MKDIR_CMD) obj

obj/oop:
	$(MKDIR_CMD) obj/oop

# Run tests
test: jawa$(BIN_EXT) examples
	@echo "=== Testing Native Compilation ==="
	./jawa$(BIN_EXT) build examples/calculator.jw examples/calculator-test$(BIN_EXT)
	./examples/calculator-test$(BIN_EXT)
	@echo "=== All tests passed ==="

# Build all examples  
examples: jawa$(BIN_EXT)
	$(MKDIR_CMD) examples

clean:
	$(RM_DIR_CMD) obj
	$(RM_CMD) jawa$(BIN_EXT)
	$(RM_CMD) examples/*-native$(BIN_EXT) examples/*-test$(BIN_EXT)
	$(RM_CMD) examples/*.c  # Remove any leftover .c files from examples
	@echo "Clean complete"
