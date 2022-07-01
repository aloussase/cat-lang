#include <iostream>

#include "Lexer.hpp"

namespace cat
{

std::ostream&
operator<<(std::ostream& os, Token token)
{
  os << "Token{" << token.type_as_str() << "," << token.lexeme() << "}";
  return os;
}

std::string
token_type_as_str(TokenType type)
{
  switch (type)
    {
    case TokenType::NUMBER:
      return "number";
    case TokenType::DOT:
      return ".";
    case TokenType::LPAREN:
      return "(";
    case TokenType::MINUS:
      return "-";
    case TokenType::PLUS:
      return "+";
    case TokenType::RPAREN:
      return ")";
    case TokenType::STAR:
      return "*";
    case TokenType::IDENTIFIER:
      return "identifier";
    case TokenType::WALRUS:
      return ":=";
    case TokenType::END:
      return "EOF";
    }
}

std::string
Token::type_as_str()
{
  return token_type_as_str(m_type);
}

std::vector<Token>
Lexer::Lex()
{
  while (!is_at_end())
    {
      m_start = m_current;
      char c{ advance() };
      switch (c)
        {
        case '+':
          m_tokens.emplace_back(m_line, TokenType::PLUS, "+", Span{ m_start, m_current });
          break;
        case '-':
          m_tokens.emplace_back(m_line, TokenType::MINUS, "-", Span{ m_start, m_current });
          break;
        case '*':
          m_tokens.emplace_back(m_line, TokenType::STAR, "*", Span{ m_start, m_current });
          break;
        case '(':
          m_tokens.emplace_back(m_line, TokenType::LPAREN, "(", Span{ m_start, m_current });
          break;
        case ')':
          m_tokens.emplace_back(m_line, TokenType::RPAREN, ")", Span{ m_start, m_current });
          break;
        case '.':
          m_tokens.emplace_back(m_line, TokenType::DOT, ".", Span{ m_start, m_current });
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          m_tokens.push_back(number(c));
          break;
        case ':':
          {
            if (c = peek(); c != '=')
              m_diagnostics.emplace_back(m_line, "Unexpected token ':'", Span{ m_start, m_current });
            else
              {
                advance();
                m_tokens.emplace_back(m_line, TokenType::WALRUS, ":=", Span{ m_start, m_current });
              }
            break;
          }
        case ' ':
        case '\t':
        case '\r':
          break;
        case '\n':
          m_line++;
          break;
        default:
          {
            if (is_identifier_character(c))
              m_tokens.push_back(identifier(c));
            else
              m_diagnostics.emplace_back(m_line, "Invalid token '" + std::string{ c } + "'",
                                         Span{ m_start, m_current });
          }
        }
    }

  m_tokens.push_back(Token{ m_line, TokenType::END, "EOF", Span{ m_start, m_current } });
  return m_tokens;
}

bool
Lexer::is_at_end() const noexcept
{
  return !(m_current < m_source.length());
}

bool
Lexer::is_identifier_character(char c) const noexcept
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '/';
}

char
Lexer::advance() noexcept
{
  return m_source[m_current++];
}

bool
is_digit(char c)
{
  return c >= '0' && c <= '9';
}

char
Lexer::peek() noexcept
{
  return m_source[m_current];
}

Token
Lexer::number(char c) noexcept
{
  std::string result{ c };

  while (!is_at_end() && is_digit(peek()))
    {
      c = advance();
      result += c;
    }

  return { m_line, TokenType::NUMBER, result, Span{ m_start, m_current } };
}

Token
Lexer::identifier(char c) noexcept
{
  std::string result{ c };

  while (!is_at_end() && is_identifier_character(peek()))
    {
      c = advance();
      result += c;
    }

  return { m_line, TokenType::IDENTIFIER, result, Span{ m_start, m_current } };
}

}
