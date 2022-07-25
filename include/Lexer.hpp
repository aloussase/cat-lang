#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "diagnostic.hpp"
#include "span.hpp"

namespace cat
{
enum class TokenType
{
  NUMBER,
  STRING,
  PLUS,
  MINUS,
  STAR,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  DOT,
  IDENTIFIER,
  WALRUS,
  CHAR,
  LT,
  LTE,
  GT,
  GTE,
  EQ,
  END
};

[[nodiscard]] std::string token_type_as_str(TokenType);

class Token
{
public:
  constexpr
  Token(TokenType type, std::string_view lexeme, Span span)
      : m_type{ type }, m_lexeme{ lexeme }, m_span{ span }
  {
  }

  [[nodiscard]] std::string
  lexeme() const noexcept
  {
    return std::string{ m_lexeme.data(), m_lexeme.size() };
  }

  [[nodiscard]] constexpr TokenType
  type() const noexcept
  {
    return m_type;
  }

  [[nodiscard]] std::string type_as_str();

  [[nodiscard]] constexpr Span
  span() const noexcept
  {
    return m_span;
  }

private:
  TokenType m_type;
  std::string_view m_lexeme;
  Span m_span;
};

std::ostream& operator<<(std::ostream&, Token);

class Lexer
{
public:
  Lexer(const std::string& source, std::vector<Diagnostic>& errors)
      : m_source{ source }, m_diagnostics{ errors }
  {
  }

  std::vector<Token> Lex();

private:
  bool is_at_end() const noexcept;
  bool is_identifier_character(char) const noexcept;

  char advance() noexcept;
  char peek() noexcept;

  const std::string& m_source;
  std::vector<Diagnostic>& m_diagnostics;
  std::vector<Token> m_tokens = {};
  int m_current = 0;
  int m_start = 0;
};

}
