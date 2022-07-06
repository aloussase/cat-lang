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
Parser::match(TokenType type) noexcept
{
  if (peek().has_value() && peek()->type() == type)
    {
      advance();
      return true;
    }
  return false;
}

bool
Parser::matched(TokenType type) const noexcept
{
  return previous().type() == type;
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

Parser::SynchronizationPoint
Parser::error(const std::string& msg, Span span) noexcept
{
  m_diagnostics.push_back({ msg, span });
  return SynchronizationPoint{};
}

Parser::SynchronizationPoint
Parser::unterminated_statement_error(Span span) noexcept
{
  auto sync{ error("Unterminated statement", span) };
  hint("Statements must end with a '.'");
  return sync;
}

void
Parser::hint(const std::string& msg) noexcept
{
  m_diagnostics.push_back({ Diagnostic::Severity::HINT, msg });
}

void
Parser::consume(TokenType type, bool synchronize)
{
  auto token{ advance() };

  if (!token.has_value())
    {
      error("Unexpected end of file", current_span());
      goto exit;
    }

  if (token->type() == type)
    return;

  if (token->type() == TokenType::DOT)
    {
      unterminated_statement_error(token->span());
      goto exit;
    }

  if (token->type() == TokenType::END)
    error("Unexpected end of file", token->span());
  else
    error("Unexpected token '" + token->lexeme() + "'", token->span());

  hint("A " + token_type_as_str(type) + " was expected");

exit:
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

  consume(TokenType::END, false);
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
      else if (token->lexeme() == "print")
        {
          advance();
          return parse_print_stmt();
        }
    }

  auto expr{ parse_expr() };
  consume(TokenType::DOT);
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
      consume(TokenType::WALRUS);
    }
  catch (const SynchronizationPoint& ex)
    {
      hint("Maybe you meant to use the assignment operator ':='?");
      delete identifier;
      throw ex;
    }

  if (identifier->token().type() != TokenType::IDENTIFIER)
    {
      error("Expected identifier after let", identifier->token().span());
      delete identifier;
      throw SynchronizationPoint{};
    }

  auto value{ parse_expr() };

  if (!value)
    {
      auto sync{ error("Expected value at right hand of let statement", identifier->token().span()) };
      delete identifier;
      throw sync;
    }

  consume(TokenType::DOT);
  return new LetStmt{ static_cast<Identifier*>(identifier), value };
}

ast::IfStmt*
Parser::parse_if_stmt()
{
  if (is_at_end())
    throw error("Expected condition after if", current_span());

  auto condition{ parse_expr() };

  // Consume the 'then' keyword
  if (!match("then"))
    {
      auto sync{ error("Expected 'then' after if statement condition", current_span()) };
      hint("Insert 'then' to start the statement body");
      throw sync;
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
      auto sync{ error("Expected 'end' after if statement body", current_span()) };
      hint("Add 'end' to the end of the if statement");
      throw sync;
    }

  // Parse the false branch, since we did not see and 'end' above

  if (!matched("else"))
    {
      auto sync{ error("Expected else block after if", current_span()) };
      hint("Add 'else' to begin an else block");
      throw sync;
    }

  while (!is_at_end() && !match("end"))
    elseStmts.push_back(parse_stmt());

  if (!matched("end"))
    {
      auto sync{ error("Unterminated if statement", current_span()) };
      hint("Add 'end' to the end of the if statement");
      throw sync;
    }

  return new IfStmt{ condition, ifStmts, elseStmts };
}

ast::PrintStmt*
Parser::parse_print_stmt()
{
  std::vector<Expr*> exprs{};

  while (!is_at_end() && !match(TokenType::DOT))
    exprs.push_back(parse_expr());

  if (!matched(TokenType::DOT))
    throw unterminated_statement_error(current_span());

  return new PrintStmt{ std::move(exprs) };
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
      error("Invalid start of prefix expression: '" + token->lexeme() + "'", token->span());
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
          error("Invalid start of infix expression: '" + next->lexeme() + "'", next->span());
          throw SynchronizationPoint{};
        }

      lhs = infix_parselet.value()(*this, *next, lhs);
    }

  return lhs;
}

Expr*
parse_integer([[maybe_unused]] Parser& parser, Token token)
{
  switch (token.type())
    {
    case TokenType::NUMBER:
      return new Number(token, std::stoi(token.lexeme()));
    case TokenType::CHAR:
      {
        // Take the second character, the first is '#'
        auto lexeme = token.lexeme();
        auto first_char = lexeme.data() + 1;

        if (*first_char == '\\')
          {
            switch (*(first_char + 1))
              {
              case 'n':
                return new Number(token, '\n');
              }
          }

        return new Number(token, *first_char);
      }
    default:
      assert(false && "Unhandled case in parse_integer");
    }
}

Expr*
parse_identifier([[maybe_unused]] Parser& parser, Token token)
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
            parser.error("Left side of assignment must be a variable.", lhs->token().span());
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
parse_grouping_expression(Parser& parser, [[maybe_unused]] Token token)
{
  auto expr{ parser.parse_expr(0) };
  parser.consume(TokenType::RPAREN);
  return expr;
}

}
