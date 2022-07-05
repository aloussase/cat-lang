cmake_minimum_required(VERSION 3.9)
cmake_policy(VERSION 3.9)

project(cat-lang CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_subdirectory(src)

add_subdirectory(Crow EXCLUDE_FROM_ALL)

add_subdirectory(api)
target_include_directories(cat-api PRIVATE Crow/include)
