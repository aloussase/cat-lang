#pragma once

#include <any>

namespace cat
{

namespace ast
{
class Node;
class Program;
class Stmt;
class LetStmt;
class Number;
class Identifier;
class AddExpr;
class SubExpr;
class MultExpr;
class AssignExpr;
}

/**
 * This is an abstract base class that represents an expression
 * visitor that accumulates state while visiting expressions.
 */
class ExprVisitor
{
public:
  virtual ~ExprVisitor() noexcept{};

  virtual std::any VisitProgram(cat::ast::Program&) = 0;
  virtual std::any VisitStmt(cat::ast::Stmt&) = 0;
  virtual std::any VisitLetStmt(cat::ast::LetStmt&) = 0;
  virtual std::any VisitNumber(cat::ast::Number&) = 0;
  virtual std::any VisitIdentifier(cat::ast::Identifier&) = 0;
  virtual std::any VisitAddExpr(cat::ast::AddExpr&) = 0;
  virtual std::any VisitSubExpr(cat::ast::SubExpr&) = 0;
  virtual std::any VisitMultExpr(cat::ast::MultExpr&) = 0;
  virtual std::any VisitAssignExpr(cat::ast::AssignExpr&) = 0;
};

}
