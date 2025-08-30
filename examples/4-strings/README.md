# 4. String Operations

This directory contains examples demonstrating advanced string manipulation in Jawa.

## Examples:

### `string_operations.jw`
Comprehensive string operations with both English and Javanese syntax.
- **Features**: Length calculation, case conversion, substring, search/replace, splitting
- **Methods**: Both English (.toUpperCase(), .toLowerCase()) and Javanese (.menyang_gedhe(), .menyang_cilik())
- **Tested**: ✅ Working - Shows extensive string manipulation capabilities

### `string_enhanced_demo.jw`
Enhanced string demonstration with advanced features.
- **Features**: Advanced string processing
- **Tested**: Available

## Key String Features Demonstrated:
- **Length**: `.length` property and `.dawane()` method
- **Case Conversion**: 
  - English: `.toUpperCase()`, `.toLowerCase()`
  - Javanese: `.menyang_gedhe()`, `.menyang_cilik()`
- **Substring**: `.substring()` and `.potong()`
- **Search/Replace**: `.replace()` and `.ganti()`
- **Splitting**: `.split()` and `.pisah()`
- **Method Chaining**: Combining multiple string operations
- **Template Processing**: Variable substitution in strings

## Example String Operations:
```jawa
string teks = "Jawa Programming Language";

// Length
int panjang = teks.length;

// Case conversion
string gedhe = teks.toUpperCase();      // English
string gedhe2 = teks.menyang_gedhe();   // Javanese

// Substring
string bagian = teks.substring(0, 4);   // "Jawa"
string potong = teks.potong(5, 16);     // "Programming"

// Replace
string ganti = teks.replace("Jawa", "Java");
string ganti2 = teks.ganti("Language", "Bahasa");

// Split
string[] pecah = teks.split(" ");       // ["Jawa", "Programming", "Language"]
```

## How to Run:
```bash
cd /path/to/jawa
./jawa run examples/4-strings/[filename].jw
```

## Related Features:
- Dual-language syntax support
- Method chaining
- String arrays
- Template processing
