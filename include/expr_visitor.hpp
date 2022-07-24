#pragma once

#include <any>

#include "forward.hpp"

namespace cat
{

/**
 * This is an abstract base class that represents an expression
 * visitor that accumulates state while visiting expressions.
 */
class ExprVisitor
{
public:
  virtual ~ExprVisitor() noexcept{};

  virtual std::any VisitNumber(cat::ast::Number&) = 0;
  virtual std::any VisitString(cat::ast::String&) = 0;
  virtual std::any VisitIdentifier(cat::ast::Identifier&) = 0;
  virtual std::any VisitAddExpr(cat::ast::AddExpr&) = 0;
  virtual std::any VisitSubExpr(cat::ast::SubExpr&) = 0;
  virtual std::any VisitMultExpr(cat::ast::MultExpr&) = 0;
  virtual std::any VisitAssignExpr(cat::ast::AssignExpr&) = 0;
  virtual std::any VisitComparisonExpr(cat::ast::ComparisonExpr&) = 0;
};

}
