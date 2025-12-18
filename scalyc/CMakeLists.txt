cmake_minimum_required(VERSION 3.16)
project(scalyc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find LLVM
find_package(LLVM 18 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Source files
set(SOURCES
    main.cpp
    Lexer.cpp
)

add_executable(scalyc ${SOURCES})

# Link LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    orcjit
    native
)

target_link_libraries(scalyc ${LLVM_LIBS})
