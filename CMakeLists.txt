set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -fPIC" )

cmake_minimum_required(VERSION 3.12)

project(quantum_tree_generator C CXX)

set(CMAKE_OSX_ARCHITECTURES x86_64)
set(INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)

include(./cmake/CPM.cmake) # Package manager for simple requirements.

find_package(gmp REQUIRED)
find_package(GSL REQUIRED)

CPMAddPackage("gh:pybind/pybind11@2.10.0")  # pybind11, essential

add_subdirectory(src)

pybind11_add_module(_qtg_bindings src/bindings/_qtg_bindings.cpp)
target_include_directories(_qtg_bindings PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(_qtg_bindings PUBLIC common generator combo qtg_lib expknap)

# just more extensive warnings to spot bugs
target_compile_options(
        _qtg_bindings PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>")
target_compile_definitions(_qtg_bindings PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
# move the binary `_bindings` to its destination in the Python module
install(TARGETS _qtg_bindings DESTINATION ./pysrc/qtg/bindings/)