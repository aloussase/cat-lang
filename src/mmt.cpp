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

  Parser parser{ tokens };
  auto expr{ parser.Parse() };

  MIPSTranspiler transpiler{ expr };
  return transpiler.Transpile();
}

}
