#!/bin/sh
###############################################################################
# @file ejecutar.sh
# @brief Build and run AES-128 demonstration application
#
# This script performs the complete build and execution cycle:
# 1. Creates necessary directories (obj, bin)
# 2. Cleans previous build artifacts
# 3. Generates Makefile with appropriate compiler flags
# 4. Compiles the application with hardware acceleration flags
# 5. Executes the compiled binary with test data from aes.ent
#
# Requirements:
#   - GCC or Clang with AES-NI and AVX2 support
#   - makeMakefile.sh script
#   - aes.ent input file with test data
#
# Usage: ./ejecutar.sh
###############################################################################

set -e  # Exit immediately if a command fails

echo "🔹 Preparando entorno..."

# Create directories for build artifacts
mkdir -p obj  # Object files directory
mkdir -p bin  # Binary executable directory

echo "🔹 Limpiando..."
# Remove old object files and executable
rm -f obj/*.o     # Clean previous object files
rm -f bin/cifrador # Remove old executable
echo "✔ Limpieza completa"

echo "🔹 Construyendo Makefile..."
# Set compiler flags for hardware acceleration:
# -mavx2: Enable AVX2 SIMD instructions (vector operations)
# -maes: Enable AES-NI hardware acceleration instructions
export FLAGS_CC="-mavx2 -maes"

# Generate Makefile using makeMakefile.sh script
# -c ./src/       : Source files directory
# -i ./lib/       : Include/header directory
# -a ./app/cifrador.c : Main application file
# -o ./obj        : Object output directory
# -b ./bin/cifrador : Binary output path
./makeMakefile.sh \
  -c ./src/ \
  -i ./lib/ \
  -a ./app/cifrador.c \
  -o ./obj \
  -b ./bin/cifrador
echo "✔ Makefile generado"

echo "🔹 Compilando..."
# Build the project using generated Makefile
make
echo "✔ Compilación exitosa"

echo "🔹 Ejecutando..."
# Run the compiled binary with test data from aes.ent file
./bin/cifrador < entrada.ent

echo "✅ Proceso finalizado correctamente"