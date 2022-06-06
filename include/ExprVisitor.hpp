#pragma once

namespace mmt
{

namespace ast
{
class Number;
class AddExpr;
class SubExpr;
class MultExpr;
}

/**
 * This is an abstract base class that represents an expression
 * visitor that accumulates state while visiting expressions.
 */
class ExprVisitor
{
public:
  virtual ~ExprVisitor() noexcept{};

  virtual void VisitNumberExpr(mmt::ast::Number&) = 0;
  virtual void VisitAddExpr(mmt::ast::AddExpr&) = 0;
  virtual void VisitSubExpr(mmt::ast::SubExpr&) = 0;
  virtual void VisitMultExpr(mmt::ast::MultExpr&) = 0;
};

}
