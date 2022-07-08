add_library(cat-lang
  ast.cpp
  mips_transpiler.cpp
  lexer.cpp
  parser.cpp
  diagnostic.cpp
  cat.cpp
)

target_link_libraries(cat-lang PUBLIC fmt::fmt)

target_compile_options(cat-lang PUBLIC "-Wall" "-Wextra")
target_compile_definitions(cat-lang PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

add_executable(cat-exe
  main.cpp
)

target_link_libraries(cat-exe PRIVATE cat-lang)
