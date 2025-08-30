# 5. File Operations

This directory contains examples demonstrating file input/output operations in Jawa.

## Examples:

### `file_minimal.jw`
Minimal file operations example.
- **Features**: Basic file I/O
- **Tested**: ✅ Working

### `file_operations.jw`
Comprehensive file operations demonstration.
- **Features**: Reading, writing, file manipulation
- **Tested**: Available

### `file_operations_fix.jw`
Fixed version of file operations with error handling.
- **Features**: Robust file operations with proper error checking
- **Tested**: Available

### `file_operations_simple.jw`
Simplified file operations for learning.
- **Features**: Basic file read/write operations
- **Tested**: Available

## Key File Features Demonstrated:
- **File Reading**: Reading content from files
- **File Writing**: Writing data to files
- **File Existence**: Checking if files exist
- **Error Handling**: Proper handling of file operation errors
- **File Paths**: Working with file paths and directories

## Example File Operations:
```jawa
// Reading a file
string konten = baca_file("input.txt");

// Writing to a file
tulis_file("output.txt", "Hello, World!");

// Check if file exists
bool ada = file_exists("test.txt");

// File operations with error handling
if (file_exists("data.txt")) {
    string data = baca_file("data.txt");
    tulis("File content: " + data);
} else {
    tulis("File not found!");
}
```

## How to Run:
```bash
cd /path/to/jawa
./jawa run examples/5-file-operations/[filename].jw
```

## Prerequisites:
- Write permissions in the working directory
- Sample files for reading operations (created automatically in some examples)

## Related Features:
- Error handling
- String operations
- Boolean operations
