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
  RPAREN,
  DOT,
  END
};

[[nodiscard]] std::string token_type_as_str(TokenType);

class Token
{
public:
  Token(int line, TokenType type, const std::string& lexeme)
      : line_{ line }, type_{ type }, lexeme_{ lexeme }
  {
  }

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

  [[nodiscard]] std::string type_as_str();

  [[nodiscard]] int
  line() const noexcept
  {
    return line_;
  }

private:
  int line_;
  TokenType type_;
  std::string lexeme_;
};

std::ostream& operator<<(std::ostream&, Token);

class Lexer
{
public:
  class InvalidTokenException : public std::exception
  {
  public:
    InvalidTokenException(int line, const std::string& msg)
        : message{ "Invalid token at line " + std::to_string(line) + ": " + msg }
    {
    }

    const char*
    what() const noexcept override
    {
      return message.c_str();
    }

  private:
    std::string message;
  };

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
  int line_ = 0;
};

}
