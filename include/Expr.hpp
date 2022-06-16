#pragma once

#include <any>
#include <cassert>
#include <vector>

#include "ExprVisitor.hpp"
#include "Lexer.hpp"

namespace cat
{

namespace ast
{

// Node
class Node
{
public:
  virtual ~Node() noexcept{};

  virtual std::any Accept(ExprVisitor&) = 0;
};

class Stmt : public Node
{
public:
  std::any Accept(ExprVisitor&) override;
};

class Program final : public Node
{
public:
  Program() : stmts_{} {}

  virtual ~Program()
  {
    for (Stmt* stmt : stmts_)
      delete stmt;
  }

  std::any Accept(ExprVisitor&) override;

  void
  add_stmt(Stmt* stmt) noexcept
  {
    assert(stmt != nullptr);
    stmts_.push_back(stmt);
  }

  [[nodiscard]] std::vector<Stmt*>
  stmts() const noexcept
  {
    return stmts_;
  }

private:
  std::vector<Stmt*> stmts_;
};

// Expr
class Expr : public Stmt
{
public:
  Expr(Token token) : token_(token) {}

  [[nodiscard]] virtual Token
  token() const noexcept
  {
    return token_;
  }

protected:
  Token token_;
};

/**
 * Represents a number literal
 */
class Number final : public Expr
{
public:
  Number(Token token, int value) : Expr{ token }, value_{ value } {}

  int
  value()
  {
    return value_;
  }

  std::any Accept(ExprVisitor&) override;

private:
  int value_ = 0;
};

class Identifier final : public Expr
{
public:
  Identifier(Token token) : Expr{ token } {}

  std::string
  name()
  {
    return token_.lexeme();
  }

  std::any Accept(ExprVisitor&) override;
};

class BinaryExpr : public Expr
{
public:
  BinaryExpr(Token token, Expr* lhs, Expr* rhs) : Expr{ token }, lhs_{ lhs }, rhs_{ rhs } {}

  virtual ~BinaryExpr() noexcept
  {
    delete lhs_;
    delete rhs_;
  };

  BinaryExpr(const BinaryExpr&) = delete;
  BinaryExpr& operator=(const BinaryExpr&) = delete;

  BinaryExpr(BinaryExpr&& be) : Expr{ be.token() }, lhs_{ be.lhs_ }, rhs_{ be.rhs_ }
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
  AddExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}

  std::any Accept(ExprVisitor&) override;
};

// SubExpr x - y
class SubExpr final : public BinaryExpr
{
public:
  SubExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}

  std::any Accept(ExprVisitor&) override;
};

// MultExpr x * y
class MultExpr final : public BinaryExpr
{
public:
  MultExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}

  std::any Accept(ExprVisitor&) override;
};

}
}
