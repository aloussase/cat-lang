#pragma once

#include <memory>

/*
 * Expr := AddExpr | SubExpr | MultExpr | Number | '(' Expr ')'
 * AddExpr := Expr '+' Expr
 * SubExpr := Expr '-' Expr
 * MultExpr := Expr '*' Expr
 * Number := (0 | 1 | 2 | 3 | ... | 9)+
 */

namespace mmt
{

// Forward declarations
class ExprVisitor;

namespace ast
{

// Expr
class Expr
{
public:
  virtual ~Expr() noexcept{};

  virtual void Accept(ExprVisitor&) = 0;
};

/**
 * Represents a number literal
 */
class Number final : public Expr
{
public:
  Number(int value) : value_{ value } {}

  int
  value()
  {
    return value_;
  }

  void Accept(ExprVisitor&) override;

private:
  int value_ = 0;
};

class BinaryExpr : public Expr
{
public:
  BinaryExpr(Expr* lhs, Expr* rhs) : lhs_{ lhs }, rhs_{ rhs } {}

  virtual ~BinaryExpr() noexcept
  {
    delete lhs_;
    delete rhs_;
  };

  BinaryExpr(const BinaryExpr&) = delete;
  BinaryExpr& operator=(const BinaryExpr&) = delete;

  BinaryExpr(BinaryExpr&& be) : lhs_{ be.lhs_ }, rhs_{ be.rhs_ }
  {
    be.lhs_ = nullptr;
    be.rhs_ = nullptr;
  }

  BinaryExpr&
  operator=(BinaryExpr&& be)
  {
    return { be };
  }

  virtual Expr*
  lhs()
  {
    return lhs_;
  }

  virtual Expr*
  rhs()
  {
    return rhs_;
  }

protected:
  Expr* lhs_;
  Expr* rhs_;
};

// AddExpr x + y
class AddExpr final : public BinaryExpr
{
public:
  AddExpr(Expr* lhs, Expr* rhs) : BinaryExpr{ lhs, rhs } {}

  void Accept(ExprVisitor&) override;
};

// SubExpr x - y
class SubExpr final : public BinaryExpr
{
public:
  SubExpr(Expr* lhs, Expr* rhs) : BinaryExpr{ lhs, rhs } {}

  void Accept(ExprVisitor&) override;
};

// MultExpr x * y
class MultExpr final : public BinaryExpr
{
public:
  MultExpr(Expr* lhs, Expr* rhs) : BinaryExpr{ lhs, rhs } {}

  void Accept(ExprVisitor&) override;
};

}

}
