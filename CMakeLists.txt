set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
cmake_minimum_required(VERSION 3.12)

project(quantum_tree_generator C CXX)

set(CMAKE_OSX_ARCHITECTURES x86_64)
set(INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)

find_package(gmp REQUIRED)
find_package(GSL REQUIRED)

add_subdirectory(src)
add_subdirectory(apps)