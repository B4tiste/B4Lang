cmake_minimum_required(VERSION 3.10)
project(B4lang C)

set(CMAKE_C_STANDARD 99)

# Define the executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Collect all source files
set(SOURCES
    main.c
    ast.c
    parser.c
    lexer.c
)

# Define the executable
add_executable(B4lang ${SOURCES})

# Add compiler flags
target_compile_options(B4lang PRIVATE -Wall -Wextra)
