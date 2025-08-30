# 6. Error Handling

This directory contains examples demonstrating error handling patterns in Jawa.

## Examples:

### `error_simple.jw`
Simple error handling demonstration.
- **Features**: Basic error catching and handling
- **Tested**: ✅ Working - Shows file error handling

### `error_handling.jw`
Comprehensive error handling examples.
- **Features**: Various error scenarios and handling patterns
- **Tested**: Available

### `error_handling_fix.jw`
Fixed version with improved error handling.
- **Features**: Robust error handling patterns
- **Tested**: Available

### `error_handling_fix2.jw`
Second iteration of error handling improvements.
- **Features**: Advanced error handling
- **Tested**: Available

## Key Error Handling Features Demonstrated:
- **File Errors**: Handling file not found and permission errors
- **Type Errors**: Managing type conversion errors
- **Runtime Errors**: Catching and handling runtime exceptions
- **Graceful Degradation**: Continuing execution after errors
- **Error Messages**: Providing meaningful error feedback

## Example Error Handling Patterns:
```jawa
// File error handling
if (!file_exists("nonexistent.txt")) {
    tulis("Error: File not found!");
} else {
    string content = baca_file("nonexistent.txt");
}

// Try-catch equivalent pattern
bool berhasil = coba_buka_file("test.txt");
if (!berhasil) {
    tulis("Error: Could not open file");
} else {
    tulis("File opened successfully");
}

// Validation before operations
if (angka != 0) {
    double hasil = 100.0 / angka;
} else {
    tulis("Error: Division by zero!");
}
```

## How to Run:
```bash
cd /path/to/jawa
./jawa run examples/6-error-handling/[filename].jw
```

## Common Error Scenarios:
- File not found errors
- Permission denied errors
- Division by zero
- Invalid type conversions
- Array out of bounds

## Related Features:
- File operations
- Boolean logic
- Conditional statements
- Function return values
