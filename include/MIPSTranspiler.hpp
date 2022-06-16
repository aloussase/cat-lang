#pragma once

#include <any>
#include <bitset>
#include <cassert>
#include <string>
#include <unordered_map>

#include "ExprVisitor.hpp"

namespace cat
{

// Forward declarations
class Instruction;

class register_t
{
public:
  static const int min_value = 8;
  static const int max_value = 25;
  static const int size = max_value - min_value;

  // TODO: registers from $v0 to $a3 are not used,
  //       reserving them for function calls.
  enum class name
  {
    ZERO,
    AT, // Reserved for assembler
    V0,
    V1,
    A0,
    A1,
    A2,
    A3,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    T8,
    T9,
    K1, // Reserved for kernel
    K2, // Reserved for kernel
    GP, // Global pointer
    SP, // Stack pointer
    FP, // Frame pointer
    RA  // Return address
  };

  constexpr
  register_t() noexcept : register_t{ name::T0 }
  {
  }

  constexpr
  register_t(name name) noexcept : name_{ name }
  {
  }

  constexpr
  register_t(int value) noexcept : name_{ static_cast<name>(value) }
  {
  }

  [[nodiscard]] constexpr operator int() const noexcept { return static_cast<int>(name_); }

  [[nodiscard]] operator std::string() const noexcept
  {
    switch (name_)
      {
      case name::T0:
        return "$t0";
      case name::T1:
        return "$t1";
      case name::T2:
        return "$t2";
      case name::T3:
        return "$t3";
      case name::T4:
        return "$t4";
      case name::T5:
        return "$t5";
      case name::T6:
        return "$t6";
      case name::T7:
        return "$t7";
      case name::S0:
        return "$s0";
      case name::S1:
        return "$s1";
      case name::S2:
        return "$s2";
      case name::S3:
        return "$s3";
      case name::S4:
        return "$s4";
      case name::S5:
        return "$s5";
      case name::S6:
        return "$s6";
      case name::S7:
        return "$s7";
      case name::T8:
        return "$t8";
      case name::T9:
        return "$t9";
      case name::SP:
        return "$sp";
      }

    return "unhandled register to std::string conversion";
  }

private:
  name name_;
};

class MIPSTranspiler final : public ExprVisitor
{
public:
  MIPSTranspiler(ast::Node* program) : program_{ program } {}

  ~MIPSTranspiler();

  class UnboundVariableException;

  // TODO: Check for stack overflow.
  class Stack
  {
  public:
    constexpr int
    push() noexcept
    {
      size_ += 4;
      return size_ - 4;
    }

    constexpr void
    pop() noexcept
    {
      size_ -= 4;
    }

  private:
    int size_ = {};
  };

  std::string Transpile();

  std::any VisitProgram(ast::Program&) override;
  std::any VisitStmt(ast::Stmt&) override;
  std::any VisitLetStmt(ast::LetStmt&) override;
  std::any VisitNumber(ast::Number&) override;
  std::any VisitIdentifier(ast::Identifier&) override;
  std::any VisitAddExpr(ast::AddExpr&) override;
  std::any VisitSubExpr(ast::SubExpr&) override;
  std::any VisitMultExpr(ast::MultExpr&) override;

private:
  [[nodiscard]] register_t find_register() noexcept;
  void release_register(register_t reg);

  void emit(const std::string& s) noexcept;
  void emit(const Instruction& instruction) noexcept;

  template <typename Inst, typename... Args>
  void
  emit(Args&&... args) noexcept
  {
    emit(Inst(args...));
  }

  ast::Node* program_;
  std::string result_ = {};
  std::bitset<register_t::size> registers_ = register_t::min_value;
  std::unordered_map<std::string, int> m_variables = {};
  Stack m_stack = {};
};

class MIPSTranspiler::UnboundVariableException : public std::exception
{
public:
  UnboundVariableException(int line, const std::string& msg)
      : message{ "Unbound variable at line " + std::to_string(line) + ": " + msg }
  {
  }

  const char*
  what() const noexcept override
  {
    return message.c_str();
  }

private:
  std::string message = {};
};

}
