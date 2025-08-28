#!/bin/bash
# Jawa Programming Language - Maintenance Script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🌟 Jawa Programming Language v0.25 - Maintenance Script${NC}"
echo ""

# Function to print colored status
print_status() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Check if in correct directory
if [ ! -f "jawa" ]; then
    print_error "Please run this script from the jawa project root directory"
    exit 1
fi

case "$1" in
    "clean")
        echo -e "${YELLOW}🧹 Cleaning project...${NC}"
        make clean
        rm -f build/* 2>/dev/null || true
        rm -f examples/*.c 2>/dev/null || true
        print_status "Project cleaned"
        ;;
        
    "build")
        echo -e "${YELLOW}🔨 Building project...${NC}"
        make clean
        make
        print_status "Project built successfully"
        ;;
        
    "test")
        echo -e "${YELLOW}🧪 Running all tests...${NC}"
        
        # Test working features
        echo "Testing working_features.jw..."
        ./jawa build examples/working_features.jw build/test_features
        ./build/test_features > /dev/null
        print_status "working_features.jw - PASSED"
        
        # Test quick demo
        echo "Testing quick_demo.jw..."
        ./jawa build examples/quick_demo.jw build/test_quick
        ./build/test_quick > /dev/null
        print_status "quick_demo.jw - PASSED"
        
        # Test language comparison
        echo "Testing language_comparison.jw..."
        ./jawa build examples/language_comparison.jw build/test_lang
        ./build/test_lang > /dev/null
        print_status "language_comparison.jw - PASSED"
        
        # Test localization
        echo "Testing localization_test.jw..."
        ./jawa build examples/localization_test.jw build/test_local
        ./build/test_local > /dev/null
        print_status "localization_test.jw - PASSED"
        
        # Test string operations
        echo "Testing string_operations.jw..."
        ./jawa build examples/string_operations.jw build/test_strings
        ./build/test_strings > /dev/null
        print_status "string_operations.jw - PASSED"
        
        echo ""
        print_status "All tests passed! 🎉"
        ;;
        
    "demo")
        echo -e "${YELLOW}🎮 Running comprehensive demo...${NC}"
        ./jawa build examples/working_features.jw build/demo
        echo ""
        ./build/demo
        ;;
        
    "examples")
        echo -e "${YELLOW}📚 Building all examples...${NC}"
        
        for example in examples/*.jw; do
            if [ -f "$example" ]; then
                filename=$(basename "$example" .jw)
                echo "Building $filename..."
                ./jawa build "$example" "build/$filename"
                print_status "$filename built successfully"
            fi
        done
        ;;
        
    "status")
        echo -e "${YELLOW}📊 Project Status:${NC}"
        echo ""
        echo "📁 Source files:"
        find src/ -name "*.c" | wc -l | xargs echo "   C source files:"
        find include/ -name "*.h" | wc -l | xargs echo "   Header files:"
        echo ""
        echo "📚 Examples:"
        find examples/ -name "*.jw" -not -path "examples/archived/*" | wc -l | xargs echo "   Production examples:"
        find examples/archived/ -name "*.jw" 2>/dev/null | wc -l | xargs echo "   Archived tests:"
        echo ""
        echo "🔧 Build status:"
        if [ -f "jawa" ]; then
            print_status "Compiler built and ready"
        else
            print_warning "Compiler not built - run './maintain.sh build'"
        fi
        ;;
        
    "help"|*)
        echo "Jawa Programming Language - Maintenance Commands:"
        echo ""
        echo "  ${BLUE}./maintain.sh clean${NC}     - Clean all build artifacts"
        echo "  ${BLUE}./maintain.sh build${NC}     - Build the compiler"
        echo "  ${BLUE}./maintain.sh test${NC}      - Run all example tests"
        echo "  ${BLUE}./maintain.sh demo${NC}      - Run comprehensive demo"
        echo "  ${BLUE}./maintain.sh examples${NC}  - Build all examples"
        echo "  ${BLUE}./maintain.sh status${NC}    - Show project status"
        echo "  ${BLUE}./maintain.sh help${NC}      - Show this help"
        echo ""
        echo "Examples:"
        echo "  ${YELLOW}./maintain.sh build && ./maintain.sh test${NC}"
        echo "  ${YELLOW}./maintain.sh demo${NC}"
        ;;
esac

echo ""
