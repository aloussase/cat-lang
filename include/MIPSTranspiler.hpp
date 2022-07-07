#pragma once

#include <any>
#include <bitset>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "diagnostic.hpp"
#include "node_visitor.hpp"

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
      case name::ZERO:
        return "$zero";
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
      case name::A0:
        return "$a0";
      case name::V0:
        return "$v0";
      default:
        assert(false && "unhandled register to std::string conversion");
      }
  }

private:
  name name_;
};

class Scope;

class MIPSTranspiler final : public NodeVisitor
{
public:
  MIPSTranspiler(std::unique_ptr<ast::Node> program, std::vector<Diagnostic>& diagnostics)
      : m_program{ program.release() }, m_diagnostics{ diagnostics }
  {
  }

  ~MIPSTranspiler();

  class RuntimeException
  {
  };

  // TODO: Check for stack overflow.
  class Stack
  {
  public:
    Stack(MIPSTranspiler& transpiler) : m_transpiler{ transpiler } {}

    [[nodiscard]] int push() noexcept;
    void pop() noexcept;

  private:
    int size_ = {};
    MIPSTranspiler& m_transpiler;
  };

  std::string Transpile();

  std::any VisitProgram(ast::Program&) override;
  std::any VisitStmt(ast::Stmt&) override;
  std::any VisitLetStmt(ast::LetStmt&) override;
  std::any VisitIfStmt(ast::IfStmt&) override;
  std::any VisitPrintStmt(ast::PrintStmt&) override;
  std::any VisitNumber(ast::Number&) override;
  std::any VisitIdentifier(ast::Identifier&) override;
  std::any VisitAddExpr(ast::AddExpr&) override;
  std::any VisitSubExpr(ast::SubExpr&) override;
  std::any VisitMultExpr(ast::MultExpr&) override;
  std::any VisitAssignExpr(ast::AssignExpr&) override;
  std::any VisitComparisonExpr(ast::ComparisonExpr&) override;

private:
  friend class Scope;

  [[nodiscard]] register_t find_register() noexcept;
  void release_register(register_t reg);

  [[nodiscard]] RuntimeException undeclared_variable_error(ast::Identifier&);

  void emit(const std::string& s) noexcept;
  void emit(const Instruction& instruction) noexcept;

  template <typename Inst, typename... Args>
  void
  emit(Args&&... args) noexcept
  {
    emit(Inst(args...));
  }

  std::vector<Diagnostic>&
  diagnostics()
  {
    return m_diagnostics;
  }

  [[nodiscard]] Stack&
  stack() noexcept
  {
    return m_stack;
  }

  [[nodiscard]] Scope&
  current_scope() const noexcept
  {
    return *m_scope;
  }

  void enter_scope() noexcept;
  void leave_scope() noexcept;

  std::string generate_label(const std::string& prefix = "");

  ast::Node* m_program;
  std::vector<Diagnostic>& m_diagnostics;

  std::string m_result = {};
  std::bitset<register_t::size> m_registers = register_t::min_value;
  Scope* m_scope = nullptr;
  Stack m_stack = { *this };
  int m_label_count = 0;
};

class Scope
{
public:
  Scope(MIPSTranspiler& transpiler) : Scope{ nullptr, transpiler } {}

  Scope(Scope* enclosing, MIPSTranspiler& transpiler) : m_enclosing{ enclosing }, m_transpiler{ transpiler }
  {
  }

  ~Scope()
  {
    for (decltype(m_variables)::size_type i = 0; i < m_variables.size(); i++)
      {
        m_transpiler.stack().pop();
      }
  }

  [[nodiscard]] Scope*
  enclosing() noexcept
  {
    return m_enclosing;
  }

  /**
   * Declare and initialize a variable.
   */
  void declare_and_initialize(const ast::Identifier& identifier, register_t rs) noexcept;

  /**
   * Find a variable in the current scope and return its offset into the stack.
   *
   * If the variable is not found a runtime exception is thrown.
   *
   */
  int find_variable(const ast::Identifier& identifier) const noexcept;

private:
  Scope* m_enclosing;
  MIPSTranspiler& m_transpiler;
  std::unordered_map<std::string, int> m_variables = {};
};

}
