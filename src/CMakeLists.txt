add_library(mmt
  Expr.cpp
  MIPSTranspiler.cpp
  Lexer.cpp
  Parser.cpp
  mmt.cpp
)

add_executable(mmt-exe
  main.cpp
)

target_link_libraries(mmt-exe PRIVATE mmt)
