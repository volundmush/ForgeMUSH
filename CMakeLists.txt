cmake_minimum_required(VERSION 3.17)
project(forgemush)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
#include_directories(../include ${CONAN_INCLUDE_DIRS})

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(apps)
add_subdirectory(src)
