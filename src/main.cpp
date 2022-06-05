#include <iostream>

#include "Expr.hpp"
#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"

int
main(int argc, char** argv)
{
  auto source{ "(1+2)-3" };
  mmt::Lexer lexer{ source };

  auto tokens{ lexer.Lex() };

  for (auto token : tokens)
    {
      std::cout << token << "\n";
    }

  // (1+2)-3
  auto expr{ ast::SubExpr{
      new ast::AddExpr{ new ast::Number(1), new ast::Number(2) },
      new ast::Number(3) } };

  eval::MIPSTranspiler transpiler{ expr };

  auto result = transpiler.transpile();

  std::cout << result << "\n";

  return 0;
}
