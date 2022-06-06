#pragma once

#include <string>
#include <vector>

namespace mmt
{
enum class TokenType
{
  NUMBER,
  PLUS,
  MINUS,
  STAR,
  LPAREN,
  RPAREN
};

class Token
{
public:
  Token(TokenType type, const std::string& lexeme) : type_{ type }, lexeme_{ lexeme } {}

  [[nodiscard]] std::string
  lexeme() const noexcept
  {
    return lexeme_;
  }

  [[nodiscard]] TokenType
  type() const noexcept
  {
    return type_;
  }

private:
  TokenType type_;
  std::string lexeme_;
};

std::ostream& operator<<(std::ostream&, Token);

class Lexer
{
public:
  Lexer(const std::string& source) : source_{ source }, tokens_{} {}

  std::vector<Token> Lex();

private:
  Token Number(char c) noexcept;
  bool IsAtEnd() const noexcept;
  char Advance() noexcept;
  char Peek() noexcept;

  std::vector<Token> tokens_;
  std::string source_;
  int current_ = 0;
};

}
