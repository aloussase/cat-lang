#pragma once

#include <cassert>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"

namespace mmt
{

namespace ast
{
class Expr;
}

class Parser;

ast::Expr* parse_integer(Parser&, Token);
ast::Expr* parse_grouping_expression(Parser&, Token);
ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

class Parser
{
public:
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
    precedence_[TokenType::LPAREN] = 8;

    // Register prefix parselets
    prefix_parselets_[TokenType::NUMBER] = parse_integer;
    prefix_parselets_[TokenType::LPAREN] = parse_grouping_expression;

    // Register infix parselets
    infix_parselets_[TokenType::PLUS] = parse_binary_operator;
    infix_parselets_[TokenType::MINUS] = parse_binary_operator;
    infix_parselets_[TokenType::STAR] = parse_binary_operator;
  }

  ast::Expr* Parse(int precedence = 0);

  friend ast::Expr* parse_integer(Parser&, Token);
  friend ast::Expr* parse_grouping_expression(Parser&, Token);
  friend ast::Expr* parse_binary_operator(Parser&, Token, ast::Expr*);

private:
  std::optional<PrefixParselet> get_prefix_parselet(TokenType) noexcept;
  std::optional<InfixParselet> get_infix_parselet(TokenType) noexcept;

  std::optional<Token> Advance() noexcept;
  std::optional<Token> Peek() const noexcept;

  int current_ = 0;
  std::vector<Token> tokens_;
  std::unordered_map<TokenType, int> precedence_;
  std::unordered_map<TokenType, PrefixParselet> prefix_parselets_;
  std::unordered_map<TokenType, InfixParselet> infix_parselets_;
};

}
