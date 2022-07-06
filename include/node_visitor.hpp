#pragma once

#include <any>

#include "forward.hpp"

namespace cat
{

/**
 * This is an abstract base class that represents an expression
 * visitor that accumulates state while visiting expressions.
 */
class NodeVisitor
{
public:
  virtual ~NodeVisitor() noexcept{};

  virtual std::any VisitProgram(cat::ast::Program&) = 0;
  virtual std::any VisitStmt(cat::ast::Stmt&) = 0;
  virtual std::any VisitLetStmt(cat::ast::LetStmt&) = 0;
  virtual std::any VisitIfStmt(cat::ast::IfStmt&) = 0;
  virtual std::any VisitPrintStmt(cat::ast::PrintStmt&) = 0;
  virtual std::any VisitNumber(cat::ast::Number&) = 0;
  virtual std::any VisitIdentifier(cat::ast::Identifier&) = 0;
  virtual std::any VisitAddExpr(cat::ast::AddExpr&) = 0;
  virtual std::any VisitSubExpr(cat::ast::SubExpr&) = 0;
  virtual std::any VisitMultExpr(cat::ast::MultExpr&) = 0;
  virtual std::any VisitAssignExpr(cat::ast::AssignExpr&) = 0;
};

}
