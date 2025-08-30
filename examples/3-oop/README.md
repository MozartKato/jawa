# 3. Object-Oriented Programming (OOP)

This directory contains examples demonstrating OOP features in Jawa using the `bolo` keyword.

## Examples:

### `oop_demo_fix.jw`
Complete OOP demonstration with inheritance and polymorphism.
- **Classes**: Kendaraan (Vehicle), Mobil (Car), MobilSport (SportsCar)
- **Features**: Class definition, inheritance, method overriding, `this` keyword
- **Tested**: ✅ Working - Shows vehicle creation and method calls

### `oop_demo.jw`
Basic OOP demonstration.
- **Features**: Class basics, object creation
- **Tested**: Available

### `oop_enhanced_demo.jw`
Enhanced OOP examples with advanced features.
- **Features**: Advanced OOP patterns
- **Tested**: Available

## Key OOP Features Demonstrated:
- **Class Definition**: Using `bolo` keyword
- **Properties**: Object attributes with `owahi` keyword
- **Methods**: Object functions
- **This Keyword**: `this.property` access within methods
- **Inheritance**: Class extending other classes
- **Method Overriding**: Specialized method implementations

## Example Class Syntax:
```jawa
bolo Kendaraan {
    owahi string merek;
    owahi int tahun;
    owahi string warna;
    
    gawe void info() {
        tulis("Kendaraan " + this.merek + " tahun " + tahun_str(this.tahun) + ", warna " + this.warna);
    }
    
    gawe void ubah_merek(string merek_baru) {
        this.merek = merek_baru;
    }
}
```

## How to Run:
```bash
cd /path/to/jawa
./jawa run examples/3-oop/[filename].jw
```

## Related Features:
- `bolo` - class definition keyword
- `owahi` - property declaration
- `this` - object self-reference
- `gawe` - method definition
- `bali` - return statement
