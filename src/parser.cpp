#include "Parser.hpp"
#include "ast.hpp"

using namespace cat::ast;

namespace cat
{

std::optional<Token>
Parser::advance() noexcept
{
  if (m_current < m_tokens.size())
    return m_tokens[m_current++];
  return std::nullopt;
}

std::optional<Token>
Parser::peek() const noexcept
{
  if (m_current < m_tokens.size())
    return m_tokens[m_current];
  return std::nullopt;
}

void
Parser::error(int line, const std::string& msg) noexcept
{
  m_diagnostics.push_back({ line, msg });
}

void
Parser::hint(const std::string& msg) noexcept
{
  m_diagnostics.push_back({ Diagnostic::Severity::HINT, msg });
}

void
Parser::consume(int line, TokenType type, bool synchronize)
{
  auto token{ advance() };

  if (!token.has_value())
    {
      error(line, "Unexpected end of file");
      if (synchronize)
        throw SynchronizationPoint{};
      else
        return;
    }

  assert(token.has_value());

  if (token->type() == type)
    return;

  if (token->type() == TokenType::END)
    error(token->line(), "Unexpected end of file");
  else
    error(token->line(), "Unexpected token '" + token->type_as_str() + "'");

  if (type == TokenType::DOT)
    hint("Statements must end with a '.'");
  else
    hint("A(n) " + token_type_as_str(type) + " was expected");

  if (synchronize)
    throw SynchronizationPoint{};
}

void
Parser::synchronize() noexcept
{
  while (peek().has_value() && peek()->type() != TokenType::END)
    {
      auto token{ advance() };
      if (token->type() == TokenType::DOT)
        return;
    }
}

std::optional<Parser::PrefixParselet>
Parser::get_prefix_parselet(TokenType type) noexcept
{
  if (auto found{ m_prefix_parselets.find(type) }; found == m_prefix_parselets.end())
    return std::nullopt;
  return m_prefix_parselets[type];
}

std::optional<Parser::InfixParselet>
Parser::get_infix_parselet(TokenType type) noexcept
{
  if (auto found{ m_infix_parselets.find(type) }; found == m_infix_parselets.end())
    return std::nullopt;
  return m_infix_parselets[type];
}

std::unique_ptr<Node>
Parser::Parse()
{
  std::unique_ptr<Program> program{ new Program };

  auto token{ peek() };
  while (token.has_value() && token->type() != TokenType::END)
    {
      try
        {
          if (auto stmt{ parse_stmt() }; stmt)
            {
              program->add_stmt(stmt);
            }
        }
      catch (const SynchronizationPoint& sync)
        {
          synchronize();
        }
      token = peek();
    }

  if (token.has_value())
    consume(token->line(), TokenType::END, false);
  return program;
};

Stmt*
Parser::parse_stmt()
{

  if (auto token{ peek() }; token->type() == TokenType::IDENTIFIER && token->lexeme() == "let")
    {
      advance();
      return parse_let_stmt();
    }
  else
    {
      auto expr{ parse_expr() };
      consume(token->line(), TokenType::DOT);
      return expr;
    }
}

LetStmt*
Parser::parse_let_stmt()
{
  Expr* identifier = nullptr;

  try
    {
      // Do not parse the walrus during variable declaration.
      identifier = parse_expr(m_precedence[TokenType::WALRUS] + 1);
      consume(identifier->token().line(), TokenType::WALRUS);
    }
  catch (const SynchronizationPoint& ex)
    {
      hint("Maybe you meant to use the walrus operator ':='?");
      throw ex;
    }

  if (identifier->token().type() != TokenType::IDENTIFIER)
    {
      error(identifier->token().line(), "Expected identifier after let");
      throw SynchronizationPoint{};
    }

  auto value{ parse_expr() };
  if (!value)
    {
      error(identifier->token().line(), "Expected value at right hand of let statement");
      throw SynchronizationPoint{};
    }

  consume(value->token().line(), TokenType::DOT);
  return new LetStmt{ static_cast<Identifier*>(identifier), value };
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
      error(token->line(), "Invalid start of prefix expression: '" + token->lexeme() + "'");
      throw SynchronizationPoint{};
    }

  auto lhs{ prefix_parselet.value()(*this, *token) };

  // FIXME: When an operator does not have an infix parselet for it registered
  //        a weird error about statements ending with a dot is shown.
  while (peek().has_value() && precedence < m_precedence[peek()->type()])
    {
      auto next{ advance() };
      auto infix_parselet{ get_infix_parselet(next->type()) };

      if (!infix_parselet.has_value())
        {
          error(next->line(), "Invalid start of infix expression: '" + next->lexeme() + "'");
          throw SynchronizationPoint{};
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
parse_binary_operator(Parser& parser, Token token, Expr* lhs)
{
  switch (token.type())
    {
    case TokenType::PLUS:
      {
        auto rhs{ parser.parse_expr(parser.m_precedence[TokenType::PLUS]) };
        return new AddExpr{ token, lhs, rhs };
      }
      break;
    case TokenType::MINUS:
      {
        auto rhs{ parser.parse_expr(parser.m_precedence[TokenType::MINUS]) };
        return new SubExpr{ token, lhs, rhs };
      }
      break;
    case TokenType::STAR:
      {
        auto rhs{ parser.parse_expr(parser.m_precedence[TokenType::STAR]) };
        return new MultExpr{ token, lhs, rhs };
      }
      break;
    case TokenType::WALRUS:
      {
        if (lhs->token().type() != TokenType::IDENTIFIER)
          {
            parser.error(lhs->token().line(), "Left side of assignment must be a variable.");
            throw Parser::SynchronizationPoint{};
          }
        auto rhs{ parser.parse_expr() };
        return new AssignExpr{ token, lhs, rhs };
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
  parser.consume(expr->token().line(), TokenType::RPAREN);
  return expr;
}

}
