#include "Expr.hpp"

namespace cat
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
  return visitor.VisitNumber(*this);
}

std::any
Identifier::Accept(ExprVisitor& visitor)
{
  return visitor.VisitIdentifier(*this);
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
