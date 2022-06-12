add_library(mmt
  Expr.cpp
  MIPSTranspiler.cpp
  Lexer.cpp
  Parser.cpp
  mmt.cpp
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Running in Debug mode")
  target_compile_definitions(mmt PRIVATE DEBUG)
endif()

add_executable(mmt-exe
  main.cpp
)

target_link_libraries(mmt-exe PRIVATE mmt)
