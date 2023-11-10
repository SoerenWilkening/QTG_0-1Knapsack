set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++17 -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -fPIC" )

message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

cmake_minimum_required(VERSION 3.12)

project(quantum_tree_generator C CXX)

set(CMAKE_OSX_ARCHITECTURES x86_64)
set(INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)

set(PYBIND11_PYTHON_VERSION 3)
set(PYBIND11_FINDPYTHON ON)
set (Python3_FIND_VIRTUALENV FIRST)
set (Python_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)

include(./cmake/CPM.cmake) # Package manager for simple requirements.

find_package(gmp REQUIRED)
find_package(GSL REQUIRED)

CPMAddPackage("gh:pybind/pybind11@2.10.0")  # pybind11, essential

add_subdirectory(src)