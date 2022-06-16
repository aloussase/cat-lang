#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Expr.hpp"
#include "Lexer.hpp"

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
  class SyntaxError;

  using PrefixParselet = std::function<ast::Expr*(Parser&, Token)>;
  using InfixParselet = std::function<ast::Expr*(Parser&, Token, ast::Expr*)>;

  Parser(std::vector<Token> tokens)
      : tokens_{ tokens }, prefix_parselets_{}, infix_parselets_{}, precedence_{}
  {
    // Register precedences
    precedence_[TokenType::MINUS] = 1;
    precedence_[TokenType::PLUS] = 1;
    precedence_[TokenType::STAR] = 2;
    precedence_[TokenType::NUMBER] = 3;
    precedence_[TokenType::IDENTIFIER] = 3;
    precedence_[TokenType::LPAREN] = 8;

    // Register prefix parselets
    prefix_parselets_[TokenType::NUMBER] = parse_integer;
    prefix_parselets_[TokenType::IDENTIFIER] = parse_identifier;
    prefix_parselets_[TokenType::LPAREN] = parse_grouping_expression;

    // Register infix parselets
    infix_parselets_[TokenType::PLUS] = parse_binary_operator;
    infix_parselets_[TokenType::MINUS] = parse_binary_operator;
    infix_parselets_[TokenType::STAR] = parse_binary_operator;
  }

  std::unique_ptr<ast::Node> Parse();

  friend ast::Expr* parse_integer(Parser&, Token);
  friend ast::Expr* parse_identifier(Parser&, Token);
  friend ast::Expr* parse_grouping_expression(Parser&, Token);
  friend ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

private:
  ast::Stmt* parse_stmt();
  ast::Expr* parse_expr(int precedence = 0);

  std::optional<PrefixParselet> get_prefix_parselet(TokenType) noexcept;
  std::optional<InfixParselet> get_infix_parselet(TokenType) noexcept;

  std::optional<Token> advance() noexcept;
  std::optional<Token> peek() const noexcept;
  void consume(TokenType);

  int current_ = 0;
  std::vector<Token> tokens_;
  std::unordered_map<TokenType, int> precedence_;
  std::unordered_map<TokenType, PrefixParselet> prefix_parselets_;
  std::unordered_map<TokenType, InfixParselet> infix_parselets_;
};

class Parser::SyntaxError : public std::exception
{
public:
  SyntaxError(int line, const std::string& msg)
      : message{ "Syntax error at line " + std::to_string(line) + ": " + msg }
  {
  }

  const char*
  what() const noexcept override
  {
    return message.c_str();
  }

private:
  std::string message{};
};

}
