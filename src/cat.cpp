#include <iostream>

#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"
#include "Parser.hpp"
#include "cat.hpp"

namespace cat
{

std::string
transpile(const std::string& source)
{
  std::vector<cat::Diagnostic> diagnostics{};

  auto tokens{ Lexer{ source, diagnostics }.Lex() };

#ifdef DEBUG
  for (const auto& token : tokens)
    std::cout << token << "\n";
#endif

  auto program{ Parser(tokens, diagnostics).Parse() };
  auto result{ MIPSTranspiler(std::move(program), diagnostics).Transpile() };

  for (const auto& diagnostic : diagnostics)
    {
      diagnostic.show();
    }

  return result;
}

}
