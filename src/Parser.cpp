#include "Parser.hpp"
#include "Expr.hpp"

using namespace mmt::ast;

namespace mmt
{

std::optional<Token>
Parser::Advance() noexcept
{
  if (current_ < tokens_.size())
    return tokens_[current_++];
  return std::nullopt;
}

std::optional<Token>
Parser::Peek() const noexcept
{
  if (current_ < tokens_.size())
    return tokens_[current_];
  return std::nullopt;
}

std::optional<Parser::PrefixParselet>
Parser::get_prefix_parselet(TokenType type) noexcept
{
  if (auto found{ prefix_parselets_.find(type) }; found == prefix_parselets_.end())
    {
      return std::nullopt;
    }
  return prefix_parselets_[type];
}

std::optional<Parser::InfixParselet>
Parser::get_infix_parselet(TokenType type) noexcept
{
  if (auto found{ infix_parselets_.find(type) }; found == infix_parselets_.end())
    {
      return std::nullopt;
    }
  return infix_parselets_[type];
}

Expr*
Parser::Parse(int precedence)
{
  auto token{ Advance() };
  if (!token.has_value())
    return nullptr;

  auto prefix_parselet{ get_prefix_parselet(token->type()) };
  if (!prefix_parselet.has_value())
    return nullptr;
  auto lhs{ prefix_parselet.value()(*this, *token) };

  while (Peek().has_value() && precedence < precedence_[Peek().value().type()])
    {
      auto next{ Advance() };
      auto infix_parselet{ get_infix_parselet(next->type()) };
      if (!infix_parselet.has_value())
        {
          return nullptr;
        }

      lhs = infix_parselet.value()(*this, *next, lhs);
    }

  return lhs;
}

Expr*
parse_integer(Parser& parser, Token token)
{
  return new Number(token, std::stoi(token.lexeme()));
}

Expr*
parse_binary_operator(Parser& parser, Token token, Expr* left)
{
  switch (token.type())
    {
    case TokenType::PLUS:
      {
        auto right{ parser.Parse(parser.precedence_[TokenType::PLUS]) };
        return new AddExpr{ token, left, right };
      }
      break;
    case TokenType::MINUS:
      {
        auto right{ parser.Parse(parser.precedence_[TokenType::MINUS]) };
        return new SubExpr{ token, left, right };
      }
      break;
    case TokenType::STAR:
      {
        auto right{ parser.Parse(parser.precedence_[TokenType::STAR]) };
        return new MultExpr{ token, left, right };
      }
      break;
    default:
      assert(false && "Unhandled token type in parse_binary_operator");
    }
}

Expr*
parse_grouping_expression(Parser& parser, Token token)
{
  auto expr{ parser.Parse(0) };
  auto rparen{ parser.Advance() };

  assert(rparen.has_value() && rparen->type() == TokenType::RPAREN && "Expected closing paren");
  return expr;
}

}
