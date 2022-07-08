#include "ast.hpp"

namespace cat
{

namespace ast
{

// Program
std::any
Program::Accept(NodeVisitor& visitor)
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
Stmt::Accept(NodeVisitor& visitor)
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

Expr&
LetStmt::value() const noexcept
{
  return *value_;
}

std::any
LetStmt::Accept(NodeVisitor& visitor)
{
  return visitor.VisitLetStmt(*this);
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

std::any
IfStmt::Accept(NodeVisitor& visitor)
{
  return visitor.VisitIfStmt(*this);
}

// PrintStmt
PrintStmt::~PrintStmt()
{
  for (auto expr : m_exprs)
    delete expr;
}

std::any
PrintStmt::Accept(NodeVisitor& visitor)
{
  return visitor.VisitPrintStmt(*this);
}

// Number
std::any
Number::Accept(NodeVisitor& visitor)
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
String::Accept(NodeVisitor& visitor)
{
  return visitor.VisitString(*this);
}

// Identifier
std::any
Identifier::Accept(NodeVisitor& visitor)
{
  return visitor.VisitIdentifier(*this);
}

// AddExpr
std::any
AddExpr::Accept(NodeVisitor& visitor)
{
  return visitor.VisitAddExpr(*this);
}

// SubExpr
std::any
SubExpr::Accept(NodeVisitor& visitor)
{
  return visitor.VisitSubExpr(*this);
}

// MultExpr
std::any
MultExpr::Accept(NodeVisitor& visitor)
{
  return visitor.VisitMultExpr(*this);
}

std::any
AssignExpr::Accept(NodeVisitor& visitor)
{
  return visitor.VisitAssignExpr(*this);
}

std::any
ComparisonExpr::Accept(NodeVisitor& visitor)
{
  return visitor.VisitComparisonExpr(*this);
}

} // namespace ast
}
