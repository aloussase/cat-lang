#include "MIPSTranspiler.hpp"
#include "Expr.hpp"
#include "ExprVisitor.hpp"
#include "Instruction.hpp"

#define AS_REGISTER(o) std::any_cast<register_t>(o)
#define AS_NUMBER(o) static_cast<ast::Number*>(o)

#define IS_NUMBER(o) ((o)->token().type() == TokenType::NUMBER)

namespace mmt
{

MIPSTranspiler::~MIPSTranspiler() { delete expr_; }

register_t
MIPSTranspiler::find_register() noexcept
{
  assert(!registers_.all());

  for (int register_number = register_t::min_value; register_number < register_t::max_value;
       register_number++)
    {
      if (int pos = register_number - register_t::min_value; !registers_.test(pos))
        {
          registers_ |= 1 << pos; // Mark the register as being used
          return { register_number };
        }
    }

  return { -1 };
}

void
MIPSTranspiler::release_register(register_t reg)
{
  registers_ &= ~(1 << reg);
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
  expr_->Accept(*this);
  emit("jr $ra");
  return result_;
}

std::any
MIPSTranspiler::VisitNumberExpr(ast::Number& expr)
{
  auto r{ find_register() };
  emit<Instruction::LI>(r, expr.value());
  return r;
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
