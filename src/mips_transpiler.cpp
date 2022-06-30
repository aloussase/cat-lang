#include <cassert>
#ifdef DEBUG
#include <iostream>
#endif

#include "ExprVisitor.hpp"
#include "Instruction.hpp"
#include "MIPSTranspiler.hpp"
#include "ast.hpp"

#define AS_REGISTER(o) std::any_cast<register_t>(o)
#define AS_NUMBER(o) static_cast<ast::Number*>(o)

#define IS_NUMBER(o) ((o)->token().type() == TokenType::NUMBER)

namespace cat
{

MIPSTranspiler::~MIPSTranspiler() { delete m_program; }

/*
 * Scopes
 */

void
MIPSTranspiler::enter_scope() noexcept
{
  m_scope = new Scope{ m_scope, *this };
}

void
MIPSTranspiler::leave_scope() noexcept
{
  auto old_scope{ m_scope };
  m_scope = old_scope->enclosing();
  delete old_scope;
}

void
Scope::declare_and_initialize(const ast::Identifier& identifier, register_t rs) noexcept
{
  auto offset{ m_transpiler.stack().push() };
  m_variables[identifier.name()] = offset;
  m_transpiler.emit<Instruction::SW>(AS_REGISTER(rs), offset, register_t{ register_t::name::SP });
}

int
Scope::find_variable(const ast::Identifier& identifier) const noexcept
{
  if (auto offset_ptr{ m_variables.find(identifier.name()) }; offset_ptr != m_variables.end())
    return offset_ptr->second;

  if (m_enclosing != nullptr)
    return m_enclosing->find_variable(identifier);

  return -1;
}

int
MIPSTranspiler::Stack::push() noexcept
{
  size_ += 4;
  register_t stack_register{ register_t::name::SP };
  m_transpiler.emit<Instruction::ADDI>(stack_register, stack_register, -4);
  return size_ - 4;
}

void
MIPSTranspiler::Stack::pop() noexcept
{
  size_ -= 4;
  register_t stack_register{ register_t::name::SP };
  m_transpiler.emit<Instruction::ADDI>(stack_register, stack_register, 4);
}

/*
 * Register routines
 */

register_t
MIPSTranspiler::find_register() noexcept
{
  assert(!m_registers.all());

  for (int register_number = register_t::min_value; register_number <= register_t::max_value;
       register_number++)
    {
      if (int pos = register_number - register_t::min_value; !m_registers.test(pos))
        {
#ifdef DEBUG
          std::cout << "Giving register number " << pos << "\n";
#endif
          m_registers |= 1 << pos; // Mark the register as being used
          return { register_number };
        }
    }

  return { -1 };
}

void
MIPSTranspiler::release_register(register_t reg)
{
#ifdef DEBUG
  std::cout << "Releasing register " << (reg - register_t::min_value) << "\n";
#endif
  m_registers &= ~(1 << (reg - register_t::min_value));
}

void
MIPSTranspiler::emit(const Instruction& instruction) noexcept
{
  emit(instruction.to_s());
}

void
MIPSTranspiler::emit(const std::string& s) noexcept
{
  if (auto newline{ s.rfind('\n') }; newline != std::string::npos)
    m_result += s;
  else
    m_result += s + "\n";
}

/*
 * Labels
 */

std::string
MIPSTranspiler::generate_label(const std::string& prefix)
{
  return (prefix == "" ? "L" : prefix) + std::to_string(m_label_count++);
}

/*
 * Error routines
 */

void
MIPSTranspiler::undeclared_variable_error(ast::Identifier& identifier)
{
  // TODO: attach spans to expression to have access to them here
  m_diagnostics.emplace_back(identifier.token().line(), "Unbound variable " + identifier.name(),
                             Span{ 0, 0 });
  std::string hint{ "Maybe you forgot to declare the variable?\n\n" };
  hint += "\t let " + identifier.name() + " := <value>";
  m_diagnostics.push_back({ identifier.token().line(), Diagnostic::Severity::HINT, hint, Span{ 0, 0 } });

  throw RuntimeException{};
}

std::string
MIPSTranspiler::Transpile()
{
  emit(".text");
  emit(".globl main");
  emit("main:");
  enter_scope();
  if (m_program)
    {
      try
        {
          m_program->Accept(*this);
        }
      catch (const RuntimeException& ex)
        {
        }
    }
  leave_scope();
  emit("jr    $ra");
  return m_result;
}

std::any
MIPSTranspiler::VisitProgram(ast::Program& program)
{
  for (ast::Stmt* stmt : program.stmts())
    {
      assert(stmt != nullptr);
      stmt->Accept(*this);
    }
  return {};
}

std::any
MIPSTranspiler::VisitStmt(ast::Stmt& stmt)
{
  stmt.Accept(*this);
  return {};
}

std::any
MIPSTranspiler::VisitLetStmt(ast::LetStmt& letStmt)
{
  auto rs{ AS_REGISTER(letStmt.value().Accept(*this)) };
  current_scope().declare_and_initialize(letStmt.identifier(), rs);
  release_register(rs);
  return {};
}

std::any
MIPSTranspiler::VisitIfStmt(ast::IfStmt& ifStmt)
{
  // Generate code for the condition
  auto rs{ AS_REGISTER(ifStmt.condition()->Accept(*this)) };

  auto else_label{ generate_label("ELSE") };
  auto exit_if_stmt_label{ generate_label("EXIT_IF") };

  auto has_else_branch{ ifStmt.else_branch().size() > 0 };

  // Enter a new scope for the if stmt
  enter_scope();

  // Generate code for the if branch first.

  // Jump to the else branch if condition is falsey or over the if
  // when the condition is false and there is not else branch.

  if (has_else_branch)
    emit("beq " + std::string(rs) + ", $zero, " + else_label);
  else
    emit("beq " + std::string(rs) + ", $zero, " + exit_if_stmt_label);

  // We don't need the condition register anymore.

  release_register(rs);

  for (const auto& stmt : ifStmt.if_branch())
    stmt->Accept(*this);

  // Jump over the code for the else branch
  if (has_else_branch)
    emit("j " + exit_if_stmt_label);

  // Now generate code for the else branch
  if (has_else_branch)
    emit(else_label + ":");

  for (const auto& stmt : ifStmt.else_branch())
    stmt->Accept(*this);

  emit(exit_if_stmt_label + ":");

  leave_scope();
  return {};
}

std::any
MIPSTranspiler::VisitNumber(ast::Number& expr)
{
  auto r{ find_register() };
  emit<Instruction::LI>(r, expr.value());
  return r;
}

std::any
MIPSTranspiler::VisitIdentifier(ast::Identifier& identifier)
{
  if (auto offset{ current_scope().find_variable(identifier) }; offset != -1)
    {
      auto rs{ find_register() };
      emit<Instruction::LW>(rs, offset, register_t{ register_t::name::SP });
      return rs;
    }
  else
    {
      undeclared_variable_error(identifier);
    }
}

std::any
MIPSTranspiler::VisitAddExpr(ast::AddExpr& expr)
{
  register_t lhs{ AS_REGISTER(expr.lhs()->Accept(*this)) };

  if (IS_NUMBER(expr.rhs()))
    {
      emit<Instruction::ADDI>(lhs, lhs, AS_NUMBER(expr.rhs())->value());
    }
  else
    {
      register_t rhs{ AS_REGISTER(expr.rhs()->Accept(*this)) };
      emit<Instruction::ADD>(lhs, lhs, rhs);
      release_register(rhs);
    }

  return lhs;
}

std::any
MIPSTranspiler::VisitSubExpr(ast::SubExpr& expr)
{
  register_t lhs{ AS_REGISTER(expr.lhs()->Accept(*this)) };

  if (IS_NUMBER(expr.rhs()))
    {
      emit<Instruction::ADDI>(lhs, lhs, -AS_NUMBER(expr.rhs())->value());
    }
  else
    {
      register_t rhs{ AS_REGISTER(expr.rhs()->Accept(*this)) };
      emit<Instruction::SUB>(lhs, lhs, rhs);
      release_register(rhs);
    }

  return lhs;
}

std::any
MIPSTranspiler::VisitMultExpr(ast::MultExpr& expr)
{
  register_t lhs{ AS_REGISTER(expr.lhs()->Accept(*this)) };
  register_t rhs{ AS_REGISTER(expr.rhs()->Accept(*this)) };

  emit<Instruction::MULT>(lhs, rhs);
  emit<Instruction::MFLO>(lhs);

  release_register(rhs);
  return lhs;
}

std::any
MIPSTranspiler::VisitAssignExpr(ast::AssignExpr& expr)
{
  auto identifier{ static_cast<ast::Identifier*>(expr.lhs()) };

  if (auto offset{ current_scope().find_variable(*identifier) }; offset != -1)
    {
      auto rs{ AS_REGISTER(expr.rhs()->Accept(*this)) };

      emit<Instruction::SW>(rs, offset, register_t{ register_t::name::SP });

      return rs;
    }
  else
    {
      undeclared_variable_error(*identifier);
    }
}

}
