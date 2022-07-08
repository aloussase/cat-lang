#include <cassert>
#include <iostream>

#include "Lexer.hpp"

#define CURRENT_CHAR (std::string_view{ &m_source[m_current], 1 })
#define PREV_CHAR (std::string_view{ &m_source[m_current - 1], 1 })

#define CURRENT_SPAN (Span{ m_start, m_current })
#define TOKEN_LENGTH (m_current - m_start)

namespace cat
{

std::ostream&
operator<<(std::ostream& os, Token token)
{
  os << "Token{" << token.type_as_str() << "," << token.lexeme() << "}";
  return os;
}

bool
is_digit(char c)
{
  return c >= '0' && c <= '9';
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
    case TokenType::LT:
      return "<";
    case TokenType::LTE:
      return "<=";
    case TokenType::EQ:
      return "=";
    case TokenType::GT:
      return ">";
    case TokenType::GTE:
      return ">=";
    case TokenType::CHAR:
      return "char";
    case TokenType::END:
      return "EOF";
    default:
      assert(false && "Unhandled token type");
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
      auto start{ &m_source[m_current] };

      switch (char c{ advance() }; c)
        {
        case '+':
          m_tokens.emplace_back(TokenType::PLUS, PREV_CHAR, CURRENT_SPAN);
          break;
        case '-':
          m_tokens.emplace_back(TokenType::MINUS, PREV_CHAR, CURRENT_SPAN);
          break;
        case '*':
          m_tokens.emplace_back(TokenType::STAR, PREV_CHAR, CURRENT_SPAN);
          break;
        case '(':
          m_tokens.emplace_back(TokenType::LPAREN, PREV_CHAR, CURRENT_SPAN);
          break;
        case ')':
          m_tokens.emplace_back(TokenType::RPAREN, PREV_CHAR, CURRENT_SPAN);
          break;
        case '.':
          m_tokens.emplace_back(TokenType::DOT, PREV_CHAR, CURRENT_SPAN);
          break;
        case '=':
          m_tokens.emplace_back(TokenType::EQ, PREV_CHAR, CURRENT_SPAN);
          break;
        case '>':
          {
            if (c = peek(); c == '=')
              {
                advance();
                m_tokens.emplace_back(TokenType::GTE, PREV_CHAR, CURRENT_SPAN);
              }
            else
              m_tokens.emplace_back(TokenType::GT, PREV_CHAR, CURRENT_SPAN);
          }
          break;
        case '<':
          {
            if (c = peek(); c == '=')
              {
                advance();
                m_tokens.emplace_back(TokenType::LTE, PREV_CHAR, CURRENT_SPAN);
              }
            else
              m_tokens.emplace_back(TokenType::LT, PREV_CHAR, CURRENT_SPAN);
          }
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
          {
            while (!is_at_end() && is_digit(peek()))
              advance();

            std::string_view lexeme(start, TOKEN_LENGTH);
            m_tokens.emplace_back(TokenType::NUMBER, lexeme, CURRENT_SPAN);
          }
          break;
        case ':':
          {
            if (c = peek(); c != '=')
              m_diagnostics.emplace_back("Unexpected token ':'", CURRENT_SPAN);
            else
              {
                std::string_view lexeme(start, TOKEN_LENGTH);
                advance();
                m_tokens.emplace_back(TokenType::WALRUS, lexeme, CURRENT_SPAN);
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
            if (c = advance(); c == '\\')
              {
                switch (advance()) // Switch on escape character
                  {
                  case 'n':
                    m_tokens.emplace_back(TokenType::CHAR, std::string_view(start, TOKEN_LENGTH),
                                          CURRENT_SPAN);
                    break;
                  default:
                    m_diagnostics.emplace_back("Invalid escape sequence \\" + std::string{ c }, CURRENT_SPAN);
                  }
              }
            else
              m_tokens.emplace_back(TokenType::CHAR, std::string_view(start, TOKEN_LENGTH), CURRENT_SPAN);

            break;
          }
        default:
          {
            if (is_identifier_character(c))
              {
                while (!is_at_end() && is_identifier_character(peek()))
                  advance();

                std::string_view lexeme(start, TOKEN_LENGTH);
                m_tokens.emplace_back(TokenType::IDENTIFIER, lexeme, CURRENT_SPAN);
              }
            else
              m_diagnostics.emplace_back("Invalid token '" + std::string{ c } + "'", CURRENT_SPAN);
          }
        }
    }

#ifdef DEBUG
  for (const auto& token : m_tokens)
    std::cout << token.lexeme() << "\n";
#endif

  m_tokens.push_back(Token{ TokenType::END, "EOF", CURRENT_SPAN });
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

char
Lexer::peek() noexcept
{
  return m_source[m_current];
}

}
