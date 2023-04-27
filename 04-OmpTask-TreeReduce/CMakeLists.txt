cmake_minimum_required(VERSION 3.9)

project(tree-reduction C)

find_package(OpenMP REQUIRED)

set(CMAKE_C_STANDARD 99)

# 1 if the GCC is the current compiler
set(GCC $<C_COMPILER_ID:GNU>)
# 1 if the LLVM Clang is the current compiler
set(CLANG $<C_COMPILER_ID:AppleClang,Clang>)
# Set compiler warnings
add_compile_options(
    -Wall -Wextra -Wpedantic
    # String and formatting
    -Wformat=2 $<${GCC}:-Wformat-truncation=2> $<${GCC}:-Wformat-overflow=2> $<${GCC}:-Wstringop-overflow=4>
    # Implicit casts
    -Wconversion -Wsign-conversion $<${GCC}:-Wformat-signedness>
    # Variadic macros should not be empty, but most compilers accept it anyway,
    # so it is okay to disable this warning
    $<${CLANG}:-Wno-gnu-zero-variadic-macro-arguments>
)
# Source fortification is useful to catch a few runtime bugs
add_compile_definitions($<$<CONFIG:Release>:_FORTIFY_SOURCE=1>)
add_compile_definitions($<$<CONFIG:RelWithDebInfo>:_FORTIFY_SOURCE=3>)

add_executable(serial   src/matmul-serial.c)
add_executable(parallel src/matmul-parallel.c)

target_link_libraries(serial
    PUBLIC OpenMP::OpenMP_C)

target_link_libraries(parallel
    PUBLIC OpenMP::OpenMP_C)
