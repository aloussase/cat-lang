#include "mmt.hpp"

#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"
#include "Parser.hpp"

namespace mmt
{

std::string
transpile(const std::string& source)
{
  Lexer lexer{ source };
  auto tokens{ lexer.Lex() };

  if (tokens.size() == 0)
    {
      return "";
    }

  Parser parser{ tokens };
  auto expr{ parser.Parse() };

  if (expr == nullptr)
    {
      return "";
    }

  MIPSTranspiler transpiler{ expr };
  return transpiler.Transpile();
}

}
