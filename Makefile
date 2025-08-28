CC = gcc
CFLAGS = -Wall -O2 -Iinclude

SRC = src/gc.c src/value.c src/stack.c src/vartable.c src/compiler.c src/vm.c src/native.c src/main.c
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))

.PHONY: all clean test examples

all: jawa

jawa: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

# Build object files
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directory
obj:
	mkdir -p obj

# Run comprehensive tests
test: jawa examples
	@echo "=== Testing Bytecode Compilation ==="
	./jawa compile examples/comprehensive.jw examples/comprehensive.jbc
	./jawa run examples/comprehensive.jbc
	@echo "=== Testing Native Compilation ==="
	./jawa native examples/comprehensive.jw examples/comprehensive-native
	./examples/comprehensive-native
	@echo "=== All tests passed ==="

# Build all examples  
examples: jawa
	mkdir -p examples
	./jawa compile examples/comprehensive.jw examples/comprehensive.jbc 2>/dev/null || true
	./jawa native examples/comprehensive.jw examples/comprehensive-native 2>/dev/null || true

clean:
	rm -f $(OBJ) jawa
	rm -f examples/*.jbc examples/*-native examples/*.c
