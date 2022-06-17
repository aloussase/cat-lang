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
Parser::consume(int line, TokenType type)
{
  // TODO: synchronize
  auto token{ advance() };
  if (!token.has_value())
    {
      m_diagnostics.push_back({ line, "Unexpected end of file" });
      return;
    }

  if (token->type() == type)
    return;

  if (type == TokenType::END)
    m_diagnostics.push_back({ token->line(), "Unexpected end of file" });
  else
    m_diagnostics.push_back({ token->line(), "Unexpected token '" + token->type_as_str() + "'" });

  if (type == TokenType::DOT)
    {
      m_diagnostics.push_back({ Diagnostic::Severity::HINT, "Statements must end with a '.'" });
    }
  else
    m_diagnostics.push_back(
        { Diagnostic::Severity::HINT, "A(n) " + token_type_as_str(type) + " was expected" });
}

std::optional<Parser::PrefixParselet>
Parser::get_prefix_parselet(TokenType type) noexcept
{
  if (auto found{ m_prefix_parselets.find(type) }; found == m_prefix_parselets.end())
    {
      return std::nullopt;
    }
  return m_prefix_parselets[type];
}

std::optional<Parser::InfixParselet>
Parser::get_infix_parselet(TokenType type) noexcept
{
  if (auto found{ m_infix_parselets.find(type) }; found == m_infix_parselets.end())
    {
      return std::nullopt;
    }
  return m_infix_parselets[type];
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

  consume(token->line(), TokenType::END);
  return program;
};

Stmt*
Parser::parse_stmt()
{
  auto token{ peek() };
  if (!token.has_value())
    return nullptr;

  if (token->type() == TokenType::IDENTIFIER && token->lexeme() == "let")
    {
      advance();
      return parse_let_stmt();
    }

  auto expr{ parse_expr() };
  consume(token->line(), TokenType::DOT);
  return expr;
}

LetStmt*
Parser::parse_let_stmt()
{
  auto identifier{ parse_expr() };
  consume(identifier->token().line(), TokenType::WALRUS);

  if (identifier->token().type() != TokenType::IDENTIFIER)
    {
      // TODO: synchronize
      m_diagnostics.emplace_back(identifier->token().line(), "Expected identifier after let");
      return nullptr;
    }

  auto value{ parse_expr() };
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
      // TODO: synchronize
      m_diagnostics.emplace_back(token->line(),
                                 "Invalid start of prefix expression: '" + token->lexeme() + "'");
      return nullptr;
    }

  auto lhs{ prefix_parselet.value()(*this, *token) };

  while (peek().has_value() && precedence < m_precedence[peek().value().type()])
    {
      auto next{ advance() };
      auto infix_parselet{ get_infix_parselet(next->type()) };
      if (!infix_parselet.has_value())
        {
          // TODO: synchronize
          m_diagnostics.emplace_back(next->line(),
                                     "Invalid start of infix expression: '" + next->lexeme() + "'");
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
        auto right{ parser.parse_expr(parser.m_precedence[TokenType::PLUS]) };
        return new AddExpr{ token, left, right };
      }
      break;
    case TokenType::MINUS:
      {
        auto right{ parser.parse_expr(parser.m_precedence[TokenType::MINUS]) };
        return new SubExpr{ token, left, right };
      }
      break;
    case TokenType::STAR:
      {
        auto right{ parser.parse_expr(parser.m_precedence[TokenType::STAR]) };
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
  parser.consume(expr->token().line(), TokenType::RPAREN);
  return expr;
}

}
