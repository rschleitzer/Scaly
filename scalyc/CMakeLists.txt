cmake_minimum_required(VERSION 3.16)
project(scalyc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Address Sanitizer for debugging (disabled for now)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Pass LLVM library directory to code for runtime loading
# This ensures JIT can find LLVM C API symbols on all platforms
if(APPLE)
    set(LLVM_SHARED_LIB "${LLVM_LIBRARY_DIR}/libLLVM.dylib")
else()
    set(LLVM_SHARED_LIB "${LLVM_LIBRARY_DIR}/libLLVM.so")
endif()
message(STATUS "LLVM shared library: ${LLVM_SHARED_LIB}")
add_definitions(-DLLVM_SHARED_LIB_PATH="${LLVM_SHARED_LIB}")

# Path to page runtime source so AOT tests can link it with each compiled
# object. The runtime defines the scaly_alloc/release_[root_]page symbols
# that Page.scaly calls.
set(PAGE_RUNTIME_CPP "${CMAKE_CURRENT_SOURCE_DIR}/page_runtime.cpp")
add_definitions(-DPAGE_RUNTIME_CPP_PATH="${PAGE_RUNTIME_CPP}")

# Source files
set(SOURCES
    main.cpp
    Lexer.cpp
    LexerTests.cpp
    Parser.cpp
    Model.h
    ModelError.h
    ModelError.cpp
    Modeler.h
    Modeler.cpp
    Plan.h
    PlannerError.h
    PlannerError.cpp
    Planner.h
    Planner.cpp
    Emitter.h
    Emitter.cpp
    EmitterTests.h
    EmitterTests.cpp
    ModuleTests.h
    ModuleTests.cpp
    page_runtime.cpp
    # Generated test files
    ExpressionTests.h
    ExpressionTests.cpp
    DefinitionTests.h
    DefinitionTests.cpp
    ChooseTests.h
    ChooseTests.cpp
    ControlFlowTests.h
    ControlFlowTests.cpp
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
