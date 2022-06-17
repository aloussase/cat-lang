add_library(cat-lang
  ast.cpp
  MIPSTranspiler.cpp
  Lexer.cpp
  Parser.cpp
  diagnostic.cpp
  cat.cpp
)

target_compile_options(cat-lang PUBLIC "-Wall" "-Wextra")
target_compile_definitions(cat-lang PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

add_executable(cat-exe
  main.cpp
)

target_link_libraries(cat-exe PRIVATE cat-lang)
