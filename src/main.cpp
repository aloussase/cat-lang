#include <iostream>

#include "Expr.hpp"
#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"

using namespace mmt;
using namespace mmt::ast;

int
main(int argc, char** argv)
{
  auto source{ "(1+2)-3" };
  Lexer lexer{ source };

  auto tokens{ lexer.Lex() };

  for (auto token : tokens)
    {
      std::cout << token << "\n";
    }

  // (1+2)-3
  auto expr{ SubExpr{ new AddExpr{ new Number(1), new Number(2) },
                      new Number(3) } };

  MIPSTranspiler transpiler{ expr };

  auto result = transpiler.transpile();

  std::cout << result << "\n";

  return 0;
}
