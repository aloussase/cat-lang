cmake_minimum_required(VERSION 3.2)

project(mmt CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(ui)

target_include_directories(mmt PRIVATE include)
target_include_directories(mmt-exe PRIVATE include)
target_include_directories(mmt-tui PRIVATE include)
