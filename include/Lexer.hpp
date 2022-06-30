#pragma once

#include <string>
#include <vector>

#include <vector>

#include "diagnostic.hpp"
#include "span.hpp"

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
  Token(int line, TokenType type, const std::string& lexeme, Span span)
      : m_line{ line }, m_type{ type }, m_lexeme{ lexeme }, m_span{ span }
  {
  }

  [[nodiscard]] std::string
  lexeme() const noexcept
  {
    return m_lexeme;
  }

  [[nodiscard]] TokenType
  type() const noexcept
  {
    return m_type;
  }

  [[nodiscard]] std::string type_as_str();

  [[nodiscard]] int
  line() const noexcept
  {
    return m_line;
  }

  [[nodiscard]] Span
  span() const noexcept
  {
    return m_span;
  }

private:
  int m_line;
  TokenType m_type;
  // TODO: replace this with a string_view or some other kind of span
  std::string m_lexeme;
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
  Token number(char) noexcept;
  Token identifier(char) noexcept;

  bool is_at_end() const noexcept;
  bool is_identifier_character(char) const noexcept;

  char advance() noexcept;
  char peek() noexcept;

  std::string m_source;
  std::vector<Diagnostic>& m_diagnostics;
  std::vector<Token> m_tokens = {};
  int m_current = 0;
  int m_line = 1;
  int m_start = 0;
};

}
