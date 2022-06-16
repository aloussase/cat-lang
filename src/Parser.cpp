#include "Parser.hpp"
#include "Expr.hpp"

using namespace cat::ast;

namespace cat
{

std::optional<Token>
Parser::advance() noexcept
{
  if (current_ < tokens_.size())
    return tokens_[current_++];
  return std::nullopt;
}

std::optional<Token>
Parser::peek() const noexcept
{
  if (current_ < tokens_.size())
    return tokens_[current_];
  return std::nullopt;
}

void
Parser::consume(TokenType type)
{
  if (auto token{ advance() }; token->type() != type)
    {
      auto actual_type{ token->type_as_str() };
      auto expected_type{ token_type_as_str(type) };
      throw SyntaxError{ token->line(), "Unexpected token '" + actual_type + "', expected '"
                                            + expected_type + "'" };
    }
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

std::unique_ptr<Node>
Parser::Parse()
{
  std::unique_ptr<Program> program{ new Program };

  auto token{ peek() };
  while (token.has_value() && token->type() != TokenType::END)
    {
      auto stmt{ parse_stmt() };
      if (stmt == nullptr)
        {
          return nullptr;
        }
      program->add_stmt(stmt);
      token = peek();
    }

  consume(TokenType::END);
  return program;
};

Stmt*
Parser::parse_stmt()
{
  auto expr{ parse_expr(0) };
  try
    {
      consume(TokenType::DOT);
    }
  catch (...)
    {
      delete expr;
      throw;
    }
  return expr;
}

Expr*
Parser::parse_expr(int precedence)
{
  auto token{ advance() };
  if (!token.has_value())
    return nullptr;

  auto prefix_parselet{ get_prefix_parselet(token->type()) };
  if (!prefix_parselet.has_value())
    {
      throw SyntaxError{ token->line(), "Invalid start of expression: '" + token->lexeme() + "'" };
    }

  auto lhs{ prefix_parselet.value()(*this, *token) };

  while (peek().has_value() && precedence < precedence_[peek().value().type()])
    {
      auto next{ advance() };
      auto infix_parselet{ get_infix_parselet(next->type()) };
      if (!infix_parselet.has_value())
        {
          throw SyntaxError{ next->line(),
                             "Invalid start of expression: '" + next->lexeme() + "'" };
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
parse_identifier(Parser& parser, Token token)
{
  return new Identifier{ token };
}

Expr*
parse_binary_operator(Parser& parser, Token token, Expr* left)
{
  switch (token.type())
    {
    case TokenType::PLUS:
      {
        auto right{ parser.parse_expr(parser.precedence_[TokenType::PLUS]) };
        return new AddExpr{ token, left, right };
      }
      break;
    case TokenType::MINUS:
      {
        auto right{ parser.parse_expr(parser.precedence_[TokenType::MINUS]) };
        return new SubExpr{ token, left, right };
      }
      break;
    case TokenType::STAR:
      {
        auto right{ parser.parse_expr(parser.precedence_[TokenType::STAR]) };
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
  auto expr{ parser.parse_expr(0) };
  parser.consume(TokenType::RPAREN);
  return expr;
}
}
