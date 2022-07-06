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
          m_tokens.emplace_back(TokenType::PLUS, "+", Span{ m_start, m_current });
          break;
        case '-':
          m_tokens.emplace_back(TokenType::MINUS, "-", Span{ m_start, m_current });
          break;
        case '*':
          m_tokens.emplace_back(TokenType::STAR, "*", Span{ m_start, m_current });
          break;
        case '(':
          m_tokens.emplace_back(TokenType::LPAREN, "(", Span{ m_start, m_current });
          break;
        case ')':
          m_tokens.emplace_back(TokenType::RPAREN, ")", Span{ m_start, m_current });
          break;
        case '.':
          m_tokens.emplace_back(TokenType::DOT, ".", Span{ m_start, m_current });
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
              m_diagnostics.emplace_back("Unexpected token ':'", Span{ m_start, m_current });
            else
              {
                advance();
                m_tokens.emplace_back(TokenType::WALRUS, ":=", Span{ m_start, m_current });
              }
            break;
          }
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          break;
        case '#':
          {
            c = advance();
            if (c == '\\')
              {
                switch (advance())
                  {
                  case '\n':
                    m_tokens.emplace_back(TokenType::CHAR, std::string{ c }, Span{ m_start, m_current });
                    break;
                  default:
                    m_diagnostics.emplace_back("Invalid escape sequence \\" + std::string{ c },
                                               Span{ m_start, m_current });
                  }
              }
            else
              {
                m_tokens.emplace_back(TokenType::CHAR, std::string{ c }, Span{ m_start, m_current });
              }
          }
        default:
          {
            if (is_identifier_character(c))
              m_tokens.push_back(identifier(c));
            else
              m_diagnostics.emplace_back("Invalid token '" + std::string{ c } + "'",
                                         Span{ m_start, m_current });
          }
        }
    }

  m_tokens.push_back(Token{ TokenType::END, "EOF", Span{ m_start, m_current } });
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

  return { TokenType::NUMBER, result, Span{ m_start, m_current } };
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

  return { TokenType::IDENTIFIER, result, Span{ m_start, m_current } };
}

}
