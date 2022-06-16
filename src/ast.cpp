#include "ast.hpp"

namespace cat
{

namespace ast
{

// Program
std::any
Program::Accept(ExprVisitor& visitor)
{
  return visitor.VisitProgram(*this);
}

void
Program::add_stmt(Stmt* stmt) noexcept
{
  assert(stmt != nullptr);
  stmts_.push_back(stmt);
}

std::vector<Stmt*>
Program::stmts() const noexcept
{
  return stmts_;
}

// Stmt
std::any
Stmt::Accept(ExprVisitor& visitor)
{
  return visitor.VisitStmt(*this);
}

// LetStmt
LetStmt::~LetStmt()
{
  delete identifier_;
  delete value_;
}

const Identifier&
LetStmt::identifier() const noexcept
{
  return *identifier_;
}

const Expr&
LetStmt::value() const noexcept
{
  return *value_;
}

std::any
LetStmt::Accept(ExprVisitor& visitor)
{
  return visitor.VisitLetStmt(*this);
}

// Number
std::any
Number::Accept(ExprVisitor& visitor)
{
  return visitor.VisitNumber(*this);
}

int
Number::value() const noexcept
{
  return value_;
}

// Identifier
std::any
Identifier::Accept(ExprVisitor& visitor)
{
  return visitor.VisitIdentifier(*this);
}

// AddExpr
std::any
AddExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitAddExpr(*this);
}

// SubExpr
std::any
SubExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitSubExpr(*this);
}

// MultExpr
std::any
MultExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitMultExpr(*this);
}

} // namespace ast

}
