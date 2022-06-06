#pragma once

#include "Expr.hpp"
#include "ExprVisitor.hpp"

#include <string>

namespace mmt
{

class MIPSTranspiler final : public ExprVisitor
{
public:
  MIPSTranspiler(ast::Expr& expr) : expr_{ expr }, result_{ "" } {}

  std::string transpile();

  void VisitNumberExpr(ast::Number&) override;
  void VisitAddExpr(ast::AddExpr&) override;
  void VisitSubExpr(ast::SubExpr&) override;

private:
  ast::Expr& expr_;
  std::string result_;
};

}
