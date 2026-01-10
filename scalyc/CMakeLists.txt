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
