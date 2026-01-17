#!/bin/bash

# Script to apply icon resources and quality improvements to MadNotesCpp project

echo "🚀 Applying ICONOS Y MEJORAS DE CALIDAD..."

# 1. Ensure resources exist
if [ ! -f "resources/app.ico" ] || [ ! -f "resources/app.png" ]; then
    echo "⚠️  Warning: Make sure 'app.ico' and 'app.png' exist in the 'resources' folder before compiling."
fi

# 2. Resources file already created by previous steps
echo "✅ Qt resource file (resources.qrc) updated."

# 3. Source files already updated by previous steps
echo "✅ SmartTextEdit updated with native zoom and HD paste functionality."
echo "✅ MainWindow updated with window icon."

# 4. CMakeLists.txt already updated by previous steps
echo "✅ CMakeLists.txt updated to include resources."

echo ""
echo "✅ ICONOS Y MEJORAS DE CALIDAD APLICADAS."
echo "⚠️ Asegúrate de haber puesto 'app.ico' y 'app.png' en la carpeta 'resources' antes de compilar."
echo "👉 Compila: cmake --build build"
echo ""

# Optional: Run CMake configuration if build directory exists
if [ -d "build" ]; then
    echo "💡 Building project..."
    cd build && make
else
    echo "💡 To build: mkdir build && cd build && cmake .. && make"
fi