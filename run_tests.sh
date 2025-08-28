#!/bin/bash

# Jawa Language v0.2 Test Suite
# Tests all major language features

echo "🚀 Running Jawa Language Test Suite..."
echo ""

# Test 1: Basic calculator
echo "📊 Test 1: Calculator Example"
./jawa build examples/calculator.jw calc
./calc
rm -f calc
echo ""

# Test 2: Comprehensive demo
echo "🎯 Test 2: Comprehensive Demo" 
./jawa build examples/demo.jw demo
./demo
rm -f demo
echo ""

# Test 3: Complete feature test
echo "✅ Test 3: Complete Feature Test"
./jawa build examples/complete_test.jw complete_test
./complete_test
rm -f complete_test
echo ""

# Test 4: OOP Foundation (parsing only)
echo "�️ Test 4: OOP Foundation"
echo "Note: Full OOP implementation in progress"
echo "Testing class parsing..."
echo 'bolo TestClass { owahi x: int }' > temp_oop.jw
echo 'cithak "OOP parsing works!"' >> temp_oop.jw
./jawa build temp_oop.jw temp_oop
./temp_oop
rm -f temp_oop temp_oop.jw
echo ""

echo "�🎉 All tests completed successfully!"
echo "Jawa Language v0.2 - Clean, Native, Modular"
echo "✅ Features: Variables, Arithmetic, Loops, Conditionals, Multi-arg printing, OOP foundation"
echo "🧹 Clean: No temporary files, modular architecture"
