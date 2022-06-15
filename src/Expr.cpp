#include "Expr.hpp"

namespace mmt
{

namespace ast
{

std::any
Program::Accept(ExprVisitor& visitor)
{
  return visitor.VisitProgram(*this);
}

std::any
Stmt::Accept(ExprVisitor& visitor)
{
  return visitor.VisitStmt(*this);
}

std::any
Number::Accept(ExprVisitor& visitor)
{
  return visitor.VisitNumberExpr(*this);
}

std::any
AddExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitAddExpr(*this);
}

std::any
SubExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitSubExpr(*this);
}

std::any
MultExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitMultExpr(*this);
}

} // namespace ast

}
