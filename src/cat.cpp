#include "cat.hpp"

#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"
#include "Parser.hpp"

namespace cat
{

std::string
transpile(const std::string& source)
{
  try
    {
      Lexer lexer{ source };
      auto tokens{ lexer.Lex() };

      if (tokens.size() == 0)
        {
          return "";
        }

      Parser parser{ tokens };
      auto program{ parser.Parse() };

      if (program == nullptr)
        {
          return "";
        }

      return MIPSTranspiler(program.release()).Transpile();
    }
  catch (const std::exception& ex)
    {
      return ex.what();
    }
}

}
