#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"
#include "ast.hpp"

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
    m_precedence[TokenType::MINUS] = 1;
    m_precedence[TokenType::PLUS] = 1;
    m_precedence[TokenType::STAR] = 2;
    m_precedence[TokenType::NUMBER] = 3;
    m_precedence[TokenType::IDENTIFIER] = 3;
    m_precedence[TokenType::LPAREN] = 8;

    // Register prefix parselets
    m_prefix_parselets[TokenType::NUMBER] = parse_integer;
    m_prefix_parselets[TokenType::IDENTIFIER] = parse_identifier;
    m_prefix_parselets[TokenType::LPAREN] = parse_grouping_expression;

    // Register infix parselets
    m_infix_parselets[TokenType::PLUS] = parse_binary_operator;
    m_infix_parselets[TokenType::MINUS] = parse_binary_operator;
    m_infix_parselets[TokenType::STAR] = parse_binary_operator;
  }

  std::unique_ptr<ast::Node> Parse();

  friend ast::Expr* parse_integer(Parser&, Token);
  friend ast::Expr* parse_identifier(Parser&, Token);
  friend ast::Expr* parse_grouping_expression(Parser&, Token);
  friend ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

private:
  ast::Stmt* parse_stmt();
  ast::LetStmt* parse_let_stmt();
  ast::Expr* parse_expr(int precedence = 0);

  std::optional<PrefixParselet> get_prefix_parselet(TokenType) noexcept;
  std::optional<InfixParselet> get_infix_parselet(TokenType) noexcept;

  std::optional<Token> advance() noexcept;
  [[nodiscard]] std::optional<Token> peek() const noexcept;
  void consume(int, TokenType, bool synchronize = true);
  void synchronize() noexcept;

  void error(int, const std::string&) noexcept;
  void hint(const std::string&) noexcept;

  std::vector<Token> m_tokens;
  std::vector<Diagnostic>& m_diagnostics;

  std::vector<Token>::size_type m_current = 0;
  std::unordered_map<TokenType, int> m_precedence = {};
  std::unordered_map<TokenType, PrefixParselet> m_prefix_parselets = {};
  std::unordered_map<TokenType, InfixParselet> m_infix_parselets = {};
};

}
