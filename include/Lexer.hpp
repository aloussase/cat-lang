#pragma once

#include <string>
#include <vector>

#include <vector>

#include "diagnostic.hpp"

namespace cat
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
  IDENTIFIER,
  WALRUS,
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
  // TODO: replace this with a string_view or some other kind of span
  std::string lexeme_;
};

std::ostream& operator<<(std::ostream&, Token);

class Lexer
{
public:
  Lexer(const std::string& source, std::vector<Diagnostic>& errors)
      : m_source{ source }, m_diagnostics{ errors }
  {
  }

  class InvalidTokenException;

  std::vector<Token> Lex();

private:
  Token number(char) noexcept;
  Token identifier(char) noexcept;

  bool is_at_end() const noexcept;
  bool is_identifier_character(char) const noexcept;

  char advance() noexcept;
  char peek() noexcept;

  std::vector<Token> m_tokens = {};
  std::vector<Diagnostic>& m_diagnostics;
  std::string m_source;
  int m_current = 0;
  int m_line = 1;
};

class Lexer::InvalidTokenException : public std::exception
{
public:
  InvalidTokenException(int line, const std::string& msg)
  {
    message = "Invalid token at line " + std::to_string(line) + ": " + msg;
  }

  const char*
  what() const noexcept override
  {
    return message.c_str();
  }

private:
  std::string message = {};
};

}
