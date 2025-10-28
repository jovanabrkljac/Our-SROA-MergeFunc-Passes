cmake_minimum_required(VERSION 3.13)
project(MySROA LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(LLVM_DIR "/usr/lib/llvm-14/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(MySROA SHARED MySROA.cpp MergeFuncPass.cpp)


set_target_properties(MySROA PROPERTIES
    COMPILE_FLAGS "-fno-rtti -Wall"
)
