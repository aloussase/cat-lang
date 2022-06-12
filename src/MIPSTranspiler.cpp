#include "MIPSTranspiler.hpp"

namespace mmt
{

std::string
MIPSTranspiler::Transpile()
{
  result_ += ".globl main\n";
  result_ += "main:\n";

  expr_->Accept(*this);

  result_ += "jr $ra\n";
  return result_;
}

void
MIPSTranspiler::VisitNumberExpr(ast::Number& expr)
{
  auto r{ find_register() };
  result_ += "li " + ntor(r) + "," + std::to_string(expr.value());

#ifdef DEBUG
  result_ += " # Number";
#endif

  result_ += "\n";
  return_register(r);
}

void
MIPSTranspiler::VisitAddExpr(ast::AddExpr& expr)
{
  auto r1{ find_register() };
  expr.lhs()->Accept(*this);
  result_ += "move " + ntor(r1) + "," + ntor(last_register_);
#ifdef DEBUG
  result_ += " # AddExpr";
#endif
  result_ += "\n";
  release_last();

  auto r2{ find_register() };
  expr.rhs()->Accept(*this);
  result_ += "add " + ntor(r2) + "," + ntor(r1) + "," + ntor(last_register_);
#ifdef DEBUG
  result_ += " # AddExpr";
#endif
  result_ += "\n";
  release_last();

  release_register(r1);
  return_register(r2);
}

void
MIPSTranspiler::VisitSubExpr(ast::SubExpr& expr)
{
  auto r1{ find_register() };
  expr.lhs()->Accept(*this);
  result_ += "move " + ntor(r1) + "," + ntor(last_register_);

#ifdef DEBUG
  result_ += " # SubExpr";
#endif

  result_ += "\n";
  release_last();

  auto r2{ find_register() };
  expr.rhs()->Accept(*this);
  result_ += "sub " + ntor(r2) + "," + ntor(r1) + "," + ntor(last_register_);

#ifdef DEBUG
  result_ += " # SubExpr";
#endif

  result_ += "\n";
  release_last();

  release_register(r1);
  return_register(r2);
}

void
MIPSTranspiler::VisitMultExpr(ast::MultExpr& expr)
{
  auto r1{ find_register() };
  expr.lhs()->Accept(*this);
  result_ += "move " + ntor(r1) + "," + ntor(last_register_);
#ifdef DEBUG
  result_ += " # MultExpr";
#endif
  result_ += "\n";
  release_last();

  expr.rhs()->Accept(*this);
  result_ += "mult " + ntor(r1) + "," + ntor(last_register_);
#ifdef DEBUG
  result_ += " # MultExpr";
#endif
  result_ += "\n";
  release_last();

  auto r2{ find_register() };
  result_ += "mflo " + ntor(r2);
#ifdef DEBUG
  result_ += " # MultExpr";
#endif
  result_ += "\n";

  release_register(r1);
  return_register(r2);
}

}
