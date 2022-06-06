#pragma once

#include <bitset>
#include <cassert>
#include <string>

#include "Expr.hpp"
#include "ExprVisitor.hpp"

namespace mmt
{

template <int Min = 0, int Max = 8>
class Register
{
public:
  int value = 0;

  constexpr explicit Register(int N) : value{ N }
  {
    assert(N >= Min && N < Max && "Register number is not within bounds");
  }
};

class MIPSTranspiler final : public ExprVisitor
{
public:
  MIPSTranspiler(ast::Expr& expr) : expr_{ expr } {}

  std::string transpile();

  void VisitNumberExpr(ast::Number&) override;
  void VisitAddExpr(ast::AddExpr&) override;
  void VisitSubExpr(ast::SubExpr&) override;
  void VisitMultExpr(ast::MultExpr&) override;

private:
  [[nodiscard]] std::string
  ntor(Register<> reg) const noexcept
  {
    return "$t" + std::to_string(reg.value);
  }

  [[nodiscard]] Register<>
  find_register() noexcept
  {
    assert(!registers_.all());

    for (int i = 0; i < 8; i++)
      if (!registers_.test(i))
        {
          registers_ |= 1 << i;
          return Register<>{ i };
        }

    return Register<>{ -1 };
  }

  void
  release_register(Register<> reg)
  {
    registers_ &= ~(1 << reg.value);
  }

  void
  release_last()
  {
    release_register(last_register_);
  }

  constexpr void
  return_register(Register<> reg) noexcept
  {
    last_register_ = reg;
  }

  ast::Expr& expr_;
  std::string result_ = "";
  Register<> last_register_ = Register<>{ 0 };
  std::bitset<8> registers_ = 0b0000'0000;
};

}
