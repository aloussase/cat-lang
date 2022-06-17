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

MIPSTranspiler::~MIPSTranspiler() { delete program_; }

register_t
MIPSTranspiler::find_register() noexcept
{
  assert(!registers_.all());

  for (int register_number = register_t::min_value; register_number <= register_t::max_value;
       register_number++)
    {
      if (int pos = register_number - register_t::min_value; !registers_.test(pos))
        {
#ifdef DEBUG
          std::cout << "Giving register number " << pos << "\n";
#endif
          registers_ |= 1 << pos; // Mark the register as being used
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
  registers_ &= ~(1 << (reg - register_t::min_value));
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
      result_ += s;
    }
  else
    {
      result_ += s + "\n";
    }
}

std::string
MIPSTranspiler::Transpile()
{
  emit(".text");
  emit(".globl main");
  emit("main:");
  if (program_)
    program_->Accept(*this);
  emit("jr $ra");
  return result_;
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
  if (auto offset = m_variables.find(identifier.name()); offset != m_variables.end())
    {
      auto rs{ find_register() };
      emit<Instruction::LW>(rs, m_variables[identifier.name()], register_t{ register_t::name::SP });
      return rs;
    }
  else
    {
      throw UnboundVariableException{ identifier.token().line(), identifier.name() };
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

}
