#include "ast.hpp"

namespace cat
{

namespace ast
{

// Program
void
Program::Accept(StmtVisitor& visitor)
{
  visitor.VisitProgram(*this);
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

Expr&
LetStmt::value() const noexcept
{
  return *value_;
}

void
LetStmt::Accept(StmtVisitor& visitor)
{
  visitor.VisitLetStmt(*this);
}

// IfStmt

IfStmt::~IfStmt()
{
  delete m_condition;
  for (auto stmt : m_if_branch)
    delete stmt;
  for (auto stmt : m_else_branch)
    delete stmt;
}

void
IfStmt::Accept(StmtVisitor& visitor)
{
  visitor.VisitIfStmt(*this);
}

// ForStmt
void
ForStmt::Accept(StmtVisitor& visitor)
{
  visitor.VisitForStmt(*this);
}

// PrintStmt
PrintStmt::~PrintStmt()
{
  for (auto expr : m_exprs)
    delete expr;
}

void
PrintStmt::Accept(StmtVisitor& visitor)
{
  visitor.VisitPrintStmt(*this);
}

void
ExprStmt::Accept(StmtVisitor& visitor)
{
  visitor.VisitExprStmt(*this);
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

// Number
const std::string&
String::value() const noexcept
{
  return m_value;
}

std::any
String::Accept(ExprVisitor& visitor)
{
  return visitor.VisitString(*this);
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

std::any
AssignExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitAssignExpr(*this);
}

std::any
ComparisonExpr::Accept(ExprVisitor& visitor)
{
  return visitor.VisitComparisonExpr(*this);
}

} // namespace ast
}
