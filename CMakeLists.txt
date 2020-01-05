cmake_minimum_required(VERSION 3.12)
project(saltflake)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(googletest)
include(GoogleTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)