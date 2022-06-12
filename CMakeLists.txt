cmake_minimum_required(VERSION 3.3)
cmake_policy(VERSION 3.3)

project(mmt CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_subdirectory(src)
add_subdirectory(web)
