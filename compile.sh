#!/bin/bash

# Compile script for Time-Travelling File System
# Usage: ./compile.sh

echo "Compiling Time-Travelling File System..."

# Compile with C++11 standard
g++ -std=c++11 filesystem.cpp -o filesystem

if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable: filesystem"
    echo "Run with: ./filesystem"
else
    echo "Compilation failed!"
    exit 1
fi
