#include <iostream>

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

bool
Parser::is_at_end() const noexcept
{
  return !peek().has_value() || peek()->type() == TokenType::END;
}

bool
Parser::match(const std::string& lexeme) noexcept
{
  if (peek().has_value() && peek()->lexeme() == lexeme)
    {
      advance();
      return true;
    }
  return false;
}

bool
Parser::matched(const std::string& lexeme) const noexcept
{
  return previous().lexeme() == lexeme;
}

int
Parser::current_line() const noexcept
{
  if (is_at_end())
    return m_tokens[m_current - 1].line();
  return peek()->line();
}

Span
Parser::current_span() const noexcept
{
  if (is_at_end())
    return m_tokens[m_current - 1].span();
  return peek()->span();
}

Token
Parser::previous() const noexcept
{
  return m_tokens[m_current - 1];
}

void
Parser::error(int line, const std::string& msg, Span span) noexcept
{
  m_diagnostics.push_back({ line, msg, span });
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
      error(line, "Unexpected end of file", current_span());
      if (synchronize)
        throw SynchronizationPoint{};
      else
        return;
    }

  assert(token.has_value());

  if (token->type() == type)
    return;

  if (token->type() == TokenType::END)
    error(token->line(), "Unexpected end of file", token->span());
  else
    error(token->line(), "Unexpected token '" + token->lexeme() + "'", token->span());

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
  while (peek().has_value() && peek()->type() != TokenType::END && peek()->type() != TokenType::DOT)
    advance();
  // Skip the '.' or 'END'
  advance();
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
            program->add_stmt(stmt);
        }
      catch (const SynchronizationPoint& sync)
        {
#ifdef DEBUG
          std::cout << "synchronizing\n";
#endif
          synchronize();
        }
      token = peek();
    }

  consume(token->line(), TokenType::END, false);
  return program;
};

Stmt*
Parser::parse_stmt()
{
  auto token{ peek() };
  if (!token.has_value())
    throw SynchronizationPoint{};

  if (token->type() == TokenType::IDENTIFIER)
    {
      if (token->lexeme() == "let")
        {
          advance();
          return parse_let_stmt();
        }
      else if (token->lexeme() == "if")
        {
          advance();
          return parse_if_stmt();
        }
    }

  auto expr{ parse_expr() };
  consume(token->line(), TokenType::DOT);
  return expr;
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
#ifdef DEBUG
      std::cout << "parse_let_stmt emitting hint about ':=' operator\n";
#endif
      hint("Maybe you meant to use the assignment operator ':='?");
      delete identifier;
      throw ex;
    }

  if (identifier->token().type() != TokenType::IDENTIFIER)
    {
      error(identifier->token().line(), "Expected identifier after let", identifier->token().span());
      delete identifier;
      throw SynchronizationPoint{};
    }

  auto value{ parse_expr() };
  if (!value)
    {
      error(identifier->token().line(), "Expected value at right hand of let statement",
            identifier->token().span());
      delete identifier;
      throw SynchronizationPoint{};
    }

  consume(value->token().line(), TokenType::DOT);
  return new LetStmt{ static_cast<Identifier*>(identifier), value };
}

ast::IfStmt*
Parser::parse_if_stmt()
{
  if (is_at_end())
    {
      error(current_line(), "Expected condition after if", current_span());
      throw SynchronizationPoint{};
    }

  auto condition{ parse_expr() };

  // TODO: it might be better to generate tokens for keywords

  // Consume the 'then' keyword
  if (!match("then"))
    {
      error(current_line(), "Expected 'then' after if statement condition", current_span());
      hint("Insert 'then' to start the statement body");
      throw SynchronizationPoint{};
    }

  std::vector<Stmt*> ifStmts{};
  std::vector<Stmt*> elseStmts{};

  // Parse the true branch
  while (!is_at_end() && !match("else") && !match("end"))
    ifStmts.push_back(parse_stmt());

  if (matched("end"))
    return new IfStmt{ condition, ifStmts, elseStmts };

  if (is_at_end())
    {
      error(current_line(), "Expected 'end' after if statement body", current_span());
      hint("Add 'end' to the end of the if statement");
      throw SynchronizationPoint{};
    }

  // Parse the false branch, since we did not see and 'end' above

  if (!matched("else"))
    {
      error(current_line(), "Expected else block after if", current_span());
      hint("Add 'else' to begin an else block");
      throw SynchronizationPoint{};
    }

  while (!is_at_end() && !match("end"))
    elseStmts.push_back(parse_stmt());

  if (!matched("end"))
    {
      error(current_line(), "Unterminated if statement", current_span());
      hint("Add 'end' to the end of the if statement");
      throw SynchronizationPoint{};
    }

  return new IfStmt{ condition, ifStmts, elseStmts };
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
      error(token->line(), "Invalid start of prefix expression: '" + token->lexeme() + "'", token->span());
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
          error(next->line(), "Invalid start of infix expression: '" + next->lexeme() + "'", next->span());
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
            parser.error(lhs->token().line(), "Left side of assignment must be a variable.",
                         lhs->token().span());
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
