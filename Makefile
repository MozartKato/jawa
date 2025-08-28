CC = gcc
CFLAGS = -Wall -O2 -Iinclude

SRC = src/native.c src/main.c src/lexer.c src/parser.c src/statements.c src/oop/class.c
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))

.PHONY: all clean test examples

all: jawa

jawa: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

# Build object files (create nested directories)
obj/%.o: src/%.c | obj obj/oop
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directories
obj:
	mkdir -p obj

obj/oop:
	mkdir -p obj/oop

# Run tests
test: jawa examples
	@echo "=== Testing Native Compilation ==="
	./jawa build examples/calculator.jw examples/calculator-test
	./examples/calculator-test
	@echo "=== All tests passed ==="

# Build all examples  
examples: jawa
	mkdir -p examples

clean:
	rm -rf obj jawa
	rm -f examples/*-native examples/*-test
	rm -f examples/*.c  # Remove any leftover .c files from examples
	@echo "Clean complete"
