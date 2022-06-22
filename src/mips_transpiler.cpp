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

int
MIPSTranspiler::Stack::push() noexcept
{
  size_ += 4;
  register_t stack_register{ register_t{ register_t::name::SP } };
  m_transpiler.emit<Instruction::ADDI>(stack_register, stack_register, 4);
  return size_ - 4;
}

void
MIPSTranspiler::Stack::pop() noexcept
{
  size_ -= 4;
}

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
    {
      m_result += s;
    }
  else
    {
      m_result += s + "\n";
    }
}

void
MIPSTranspiler::require_variable_declared(ast::Identifier& identifier) const
{
  if (auto offset = m_variables.find(identifier.name()); offset == m_variables.end())
    {
      m_diagnostics.emplace_back(identifier.token().line(), "Unbound variable " + identifier.name());
      std::string hint{ "Maybe you forgot to declare the variable?\n\n" };
      hint += "\t let " + identifier.name() + " := <value>";
      m_diagnostics.push_back({ identifier.token().line(), Diagnostic::Severity::HINT, hint });
      throw RuntimeException{};
    }
}

std::string
MIPSTranspiler::Transpile()
{
  emit(".text");
  emit(".globl main");
  emit("main:");
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
  auto offset{ m_stack.push() };
  auto rs{ letStmt.value().Accept(*this) };

  m_variables[letStmt.identifier().name()] = offset;
  emit<Instruction::SW>(AS_REGISTER(rs), offset, register_t{ register_t::name::SP });

  release_register(AS_REGISTER(rs));
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
  require_variable_declared(identifier);

  auto rs{ find_register() };
  emit<Instruction::LW>(rs, m_variables[identifier.name()], register_t{ register_t::name::SP });
  return rs;
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

  require_variable_declared(*identifier);

  auto offset{ m_variables[identifier->name()] };
  auto rs{ AS_REGISTER(expr.rhs()->Accept(*this)) };

  emit<Instruction::SW>(rs, offset, register_t{ register_t::name::SP });

  return rs;
}

}
