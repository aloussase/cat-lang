#pragma once

namespace ast
{
class Number;
class AddExpr;
class SubExpr;
}

namespace eval
{

/**
 * This is an abstract base class that represents an expression
 * visitor that accumulates state while visiting expressions.
 */
class ExprVisitor
{
public:
  virtual ~ExprVisitor() noexcept{};

  virtual void VisitNumberExpr(ast::Number&) = 0;
  virtual void VisitAddExpr(ast::AddExpr&) = 0;
  virtual void VisitSubExpr(ast::SubExpr&) = 0;
};

}
