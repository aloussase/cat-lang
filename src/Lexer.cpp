#include <iostream>

#include "Lexer.hpp"

namespace mmt
{

std::ostream&
operator<<(std::ostream& os, Token token)
{
  os << "Token{" << token.lexeme() << "}";
  return os;
}

std::vector<Token>
Lexer::Lex()
{
  while (!IsAtEnd())
    {
      char c{ Advance() };
      switch (c)
        {
        case '+':
          tokens_.emplace_back(TokenType::PLUS, "+");
          break;
        case '-':
          tokens_.emplace_back(TokenType::MINUS, "-");
          break;
        case '*':
          tokens_.emplace_back(TokenType::STAR, "*");
          break;
        case '(':
          tokens_.emplace_back(TokenType::LPAREN, "(");
          break;
        case ')':
          tokens_.emplace_back(TokenType::RPAREN, ")");
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
          tokens_.push_back(Number(c));
          break;
        case ' ':
        case '\t':
          break;
        case '\n':
          line_++;
          break;
        default:
          std::cerr << line_ << ": Invalid token on input: " << c << "\n";
          return {};
        }
    }

  return tokens_;
}

bool
Lexer::IsAtEnd() const noexcept
{
  return !(current_ < source_.length());
}

char
Lexer::Advance() noexcept
{
  return source_[current_++];
}

bool
IsDigit(char c)
{
  return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6'
         || c == '7' || c == '8' || c == '9';
}

char
Lexer::Peek() noexcept
{
  return source_[current_];
}

Token
Lexer::Number(char c) noexcept
{
  std::string result{ c };

  while (!IsAtEnd() && IsDigit(Peek()))
    {
      c = Advance();
      result += c;
    }

  return { TokenType::NUMBER, result };
}

}
