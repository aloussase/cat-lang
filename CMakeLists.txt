cmake_minimum_required(VERSION 3.2)

project(maps CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
target_include_directories(maps PRIVATE include)
