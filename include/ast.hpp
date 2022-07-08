#pragma once

#include <any>
#include <cassert>
#include <vector>

#include "Lexer.hpp"
#include "forward.hpp"
#include "node_visitor.hpp"

namespace cat
{

namespace ast
{

// Node
class Node
{
public:
  virtual ~Node() noexcept{};

  virtual std::any Accept(NodeVisitor&) = 0;
};

class Stmt : public Node
{
public:
  std::any Accept(NodeVisitor&) override;
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

  std::any Accept(NodeVisitor&) override;

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

  std::any Accept(NodeVisitor&) override;

  [[nodiscard]] const Identifier& identifier() const noexcept;
  [[nodiscard]] Expr& value() const noexcept;

private:
  Identifier* identifier_;
  Expr* value_;
};

class IfStmt final : public Stmt
{
public:
  IfStmt(Expr* condition, std::vector<Stmt*> if_branch, std::vector<Stmt*> else_branch)
      : m_condition{ condition }, m_if_branch{ if_branch }, m_else_branch{ else_branch }
  {
  }

  ~IfStmt();

  std::any Accept(NodeVisitor&) override;

  [[nodiscard]] Expr*
  condition() const noexcept
  {
    return m_condition;
  }

  [[nodiscard]] std::vector<Stmt*>
  if_branch() const noexcept
  {
    return m_if_branch;
  }

  [[nodiscard]] std::vector<Stmt*>
  else_branch() const noexcept
  {
    return m_else_branch;
  }

private:
  Expr* m_condition;
  std::vector<Stmt*> m_if_branch;
  std::vector<Stmt*> m_else_branch;
};

// PrintStmt
class PrintStmt final : public Stmt
{
public:
  PrintStmt(std::vector<Expr*>&& exprs) : m_exprs{ std::move(exprs) } {}

  ~PrintStmt();

  std::any Accept(NodeVisitor&) override;

  [[nodiscard]] std::vector<Expr*>
  exprs() const noexcept
  {
    return m_exprs;
  }

private:
  std::vector<Expr*> m_exprs;
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

  std::any Accept(NodeVisitor&) override;

private:
  int value_ = 0;
};

class String final : public Expr
{
public:
  String(Token token, const std::string& value) : Expr{ token }, m_value{ value } {}

  [[nodiscard]] const std::string& value() const noexcept;

  std::any Accept(NodeVisitor&) override;

private:
  std::string m_value = {};
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

  std::any Accept(NodeVisitor&) override;
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
  std::any Accept(NodeVisitor&) override;
};

// SubExpr x - y
class SubExpr final : public BinaryExpr
{
public:
  SubExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}
  std::any Accept(NodeVisitor&) override;
};

// MultExpr x * y
class MultExpr final : public BinaryExpr
{
public:
  MultExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}
  std::any Accept(NodeVisitor&) override;
};

// AssignExpr x := y
class AssignExpr final : public BinaryExpr
{
public:
  AssignExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}
  std::any Accept(NodeVisitor&) override;
};

class ComparisonExpr final : public BinaryExpr
{
public:
  ComparisonExpr(Token token, Expr* lhs, Expr* rhs) : BinaryExpr{ token, lhs, rhs } {}
  std::any Accept(NodeVisitor&) override;
};

}
}
