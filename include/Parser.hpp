#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"
#include "ast.hpp"
#include "span.hpp"

namespace cat
{

class Parser;

ast::Expr* parse_integer(Parser&, Token);
ast::Expr* parse_identifier(Parser&, Token);
ast::Expr* parse_grouping_expression(Parser&, Token);
ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

class Parser
{
public:
  using PrefixParselet = std::function<ast::Expr*(Parser&, Token)>;
  using InfixParselet = std::function<ast::Expr*(Parser&, Token, ast::Expr*)>;

  class SynchronizationPoint
  {
  };

  Parser(std::vector<Token> tokens, std::vector<Diagnostic>& diagnostics)
      : m_tokens{ tokens }, m_diagnostics{ diagnostics }
  {
    // Register precedences

    // These need to be treated specially
    m_precedence[TokenType::NUMBER] = 0;
    m_precedence[TokenType::IDENTIFIER] = 0;

    m_precedence[TokenType::WALRUS] = 1;
    m_precedence[TokenType::PLUS] = 2;
    m_precedence[TokenType::MINUS] = 2;
    m_precedence[TokenType::STAR] = 3;
    m_precedence[TokenType::LPAREN] = 8;

    // Register prefix parselets
    m_prefix_parselets[TokenType::NUMBER] = parse_integer;
    m_prefix_parselets[TokenType::IDENTIFIER] = parse_identifier;
    m_prefix_parselets[TokenType::LPAREN] = parse_grouping_expression;

    // Register infix parselets
    m_infix_parselets[TokenType::PLUS] = parse_binary_operator;
    m_infix_parselets[TokenType::MINUS] = parse_binary_operator;
    m_infix_parselets[TokenType::STAR] = parse_binary_operator;
    m_infix_parselets[TokenType::WALRUS] = parse_binary_operator;
  }

  std::unique_ptr<ast::Node> Parse();

  friend ast::Expr* parse_integer(Parser&, Token);
  friend ast::Expr* parse_identifier(Parser&, Token);
  friend ast::Expr* parse_grouping_expression(Parser&, Token);
  friend ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

private:
  [[nodiscard]] ast::Stmt* parse_stmt();
  [[nodiscard]] ast::LetStmt* parse_let_stmt();
  [[nodiscard]] ast::IfStmt* parse_if_stmt();
  ast::Expr* parse_expr(int precedence = 0);

  std::optional<PrefixParselet> get_prefix_parselet(TokenType) noexcept;
  std::optional<InfixParselet> get_infix_parselet(TokenType) noexcept;

  std::optional<Token> advance() noexcept;
  [[nodiscard]] std::optional<Token> peek() const noexcept;
  void consume(int, TokenType, bool synchronize = true);
  bool is_at_end() const noexcept;
  bool match(const std::string&) noexcept;
  bool matched(const std::string&) const noexcept;
  int current_line() const noexcept;

  /// Return a Span for the curren token.
  Span current_span() const noexcept;

  // Return the previous token.
  Token previous() const noexcept;

  /// Advance the parser until the next synchronization point.
  void synchronize() noexcept;

  void error(int, const std::string&, Span) noexcept;
  void hint(const std::string&) noexcept;

  std::vector<Token> m_tokens;
  std::vector<Diagnostic>& m_diagnostics;

  std::vector<Token>::size_type m_current = 0;
  std::unordered_map<TokenType, int> m_precedence = {};
  std::unordered_map<TokenType, PrefixParselet> m_prefix_parselets = {};
  std::unordered_map<TokenType, InfixParselet> m_infix_parselets = {};
};

}
