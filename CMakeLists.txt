cmake_minimum_required(VERSION 3.10)
project(B4lang C)

set(CMAKE_C_STANDARD 99)

# Définir le répertoire de sortie des exécutables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Ajouter le répertoire include/
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collecter tous les fichiers sources dans src/
set(SOURCES
    src/main.c
    src/ast.c
    src/parser.c
    src/lexer.c
)

# Définir l'exécutable
add_executable(B4lang ${SOURCES})

# Ajouter des options de compilation
target_compile_options(B4lang PRIVATE -Wall -Wextra)
