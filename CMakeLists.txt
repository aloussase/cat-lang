cmake_minimum_required(VERSION 3.9)
cmake_policy(VERSION 3.9)

project(cat-lang CXX)

set(CMAKE_CXX_STANDARD 20)

set(CROW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Crow)
set(FMT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmt)

include_directories(include ${FMT_DIR}/include)

add_subdirectory(src)
add_subdirectory(vendor)
add_subdirectory(api)
