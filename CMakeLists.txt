cmake_minimum_required(VERSION 3.3)
cmake_policy(VERSION 3.3)

project(cat-lang CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(Crow/include)

add_subdirectory(src)
add_subdirectory(api)
