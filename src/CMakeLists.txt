add_library(cat-lang
  Expr.cpp
  MIPSTranspiler.cpp
  Lexer.cpp
  Parser.cpp
  cat.cpp
)

target_compile_definitions(cat-lang PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

add_executable(cat-exe
  main.cpp
)

target_link_libraries(cat-exe PRIVATE cat-lang)
