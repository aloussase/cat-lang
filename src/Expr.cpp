#include "Expr.hpp"
#include "ExprVisitor.hpp"

using namespace eval;

namespace ast
{

void
Number::Accept(ExprVisitor& visitor)
{
  visitor.VisitNumberExpr(*this);
}

void
AddExpr::Accept(ExprVisitor& visitor)
{
  visitor.VisitAddExpr(*this);
}

void
SubExpr::Accept(ExprVisitor& visitor)
{
  visitor.VisitSubExpr(*this);
}

} // namespace ast
