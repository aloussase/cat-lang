#include "Expr.hpp"
#include "ExprVisitor.hpp"

namespace mmt
{

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

void
MultExpr::Accept(ExprVisitor& visitor)
{
  visitor.VisitMultExpr(*this);
}

} // namespace ast

}
