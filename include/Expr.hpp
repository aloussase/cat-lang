#pragma once

#include <memory>

namespace eval
{
class ExprVisitor;
}

namespace ast
{

/*
 * Expr := AddExpr | SubExpr | MultExpr | DivExpr | Number | '(' Expr ')'
 * AddExpr := Expr '+' Expr
 * SubExpr := Expr '-' Expr
 * MultExpr := Expr '*' Expr
 * DivExpr := Expr '/' Expr
 * Number := (0 | 1 | 2 | 3 | ... | 9)+
 */

// Expr
class Expr
{
public:
  virtual ~Expr() noexcept{};

  virtual void Accept(eval::ExprVisitor&) = 0;
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

  void Accept(eval::ExprVisitor&) override;

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

  void Accept(eval::ExprVisitor&) override;
};

// SubExpr x - y
class SubExpr final : public BinaryExpr
{
public:
  SubExpr(Expr* lhs, Expr* rhs) : BinaryExpr{ lhs, rhs } {}

  void Accept(eval::ExprVisitor&) override;
};

};
