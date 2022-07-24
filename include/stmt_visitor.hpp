#pragma once

#include "forward.hpp"

namespace cat
{

class StmtVisitor
{
public:
  virtual ~StmtVisitor() noexcept{};

  virtual void VisitProgram(cat::ast::Program&) = 0;
  virtual void VisitLetStmt(cat::ast::LetStmt&) = 0;
  virtual void VisitIfStmt(cat::ast::IfStmt&) = 0;
  virtual void VisitForStmt(cat::ast::ForStmt&) = 0;
  virtual void VisitPrintStmt(cat::ast::PrintStmt&) = 0;
  virtual void VisitExprStmt(cat::ast::ExprStmt&) = 0;
};

}
