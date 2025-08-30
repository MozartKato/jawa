# 2. Functions

This directory contains examples demonstrating function definitions and usage in Jawa.

## Examples:

### `fungsi_demo.jw`
Comprehensive function demonstration with the `gawe` keyword.
- **Features**: Function definitions with `gawe`, parameters, return values
- **Functions**: `tambah()`, `kali()` with type annotations
- **Tested**: ✅ Working - Shows addition and multiplication results

### `tes_fungsi.jw`
Function testing examples.
- **Features**: Function testing and validation
- **Tested**: ✅ Working

## Key Features Demonstrated:
- **Function Definition**: Using `gawe` keyword
- **Type Annotations**: int, double, string, bool, void
- **Parameters**: Passing arguments to functions
- **Return Values**: Functions returning calculated results

## Example Function Syntax:
```jawa
gawe int tambah(int a, int b) {
    bali a + b;
}

gawe double kali(double x, double y) {
    bali x * y;
}
```

## How to run:
```bash
cd /path/to/jawa
./jawa mlayu examples/2-functions/[filename].jw
```

## Related Features:
- Type system (int, double, string, bool)
- Return statement (`bali`)
- Function calls
