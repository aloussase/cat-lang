#pragma once

#include <any>

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

  virtual std::any VisitNumberExpr(mmt::ast::Number&) = 0;
  virtual std::any VisitAddExpr(mmt::ast::AddExpr&) = 0;
  virtual std::any VisitSubExpr(mmt::ast::SubExpr&) = 0;
  virtual std::any VisitMultExpr(mmt::ast::MultExpr&) = 0;
};

}
