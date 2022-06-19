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

// Forward declarations
class Node;
class Program;
class Stmt;
class LetStmt;
class Expr;
class Identifier;
class Number;
class BinaryExpr;
class AddExpr;
class SubExpr;
class MultExpr;
class AssignExpr;

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

  ~Program()
  {
    for (Stmt* stmt : stmts_)
      delete stmt;
  }

  std::any Accept(ExprVisitor&) override;

  void add_stmt(Stmt* stmt) noexcept;
  [[nodiscard]] std::vector<Stmt*> stmts() const noexcept;

private:
  std::vector<Stmt*> stmts_;
};

class LetStmt final : public Stmt
{
public:
  LetStmt(Identifier* identifier, Expr* expr) : identifier_{ identifier }, value_{ expr } {}

  ~LetStmt();

  std::any Accept(ExprVisitor&) override;

  [[nodiscard]] const Identifier& identifier() const noexcept;
  [[nodiscard]] Expr& value() const noexcept;

private:
  Identifier* identifier_;
  Expr* value_;
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

  [[nodiscard]] int value() const noexcept;

  std::any Accept(ExprVisitor&) override;

private:
  int value_ = 0;
};

class Identifier final : public Expr
{
public:
  Identifier(Token token) : Expr{ token } {}

  [[nodiscard]] std::string
  name() const noexcept
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

// AssignExpr x := y
class AssignExpr final : public BinaryExpr
{
public:
  AssignExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}

  std::any Accept(ExprVisitor&) override;
};

}
}
