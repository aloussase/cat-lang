#include "MIPSTranspiler.hpp"

namespace eval
{

std::string
MIPSTranspiler::transpile()
{
  result_ += ".globl main\n";
  result_ += "main:\n";

  expr_.Accept(*this);

  result_ += "jr $ra\n";
  return result_;
}

void
MIPSTranspiler::VisitNumberExpr(ast::Number& expr)
{
  result_ += "li $t0, " + std::to_string(expr.value()) + "\n";
}

void
MIPSTranspiler::VisitAddExpr(ast::AddExpr& expr)
{
  expr.lhs()->Accept(*this);
  result_ += "move $t1, $t0\n";

  // The result should be in $t0.
  expr.rhs()->Accept(*this);
  result_ += "add $t0, $t0, $t1\n";
}

void
MIPSTranspiler::VisitSubExpr(ast::SubExpr& expr)
{
  expr.lhs()->Accept(*this);
  result_ += "move $t1, $t0\n";

  // The result should be in $t0.
  expr.rhs()->Accept(*this);
  result_ += "sub $t0, $t0, $t1\n";
}

}
