cmake_minimum_required(VERSION 3.2)

project(mmt CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
target_include_directories(mmt PRIVATE include)
