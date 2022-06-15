#include <iostream>

#include "Lexer.hpp"

namespace mmt
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
      return "dot";
    case TokenType::LPAREN:
      return "lparen";
    case TokenType::MINUS:
      return "minus";
    case TokenType::PLUS:
      return "plus";
    case TokenType::RPAREN:
      return "rparen";
    case TokenType::STAR:
      return "star";
    case TokenType::END:
      return "EOF";
    }
}

std::string
Token::type_as_str()
{
  return token_type_as_str(type_);
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
          tokens_.emplace_back(line_, TokenType::PLUS, "+");
          break;
        case '-':
          tokens_.emplace_back(line_, TokenType::MINUS, "-");
          break;
        case '*':
          tokens_.emplace_back(line_, TokenType::STAR, "*");
          break;
        case '(':
          tokens_.emplace_back(line_, TokenType::LPAREN, "(");
          break;
        case ')':
          tokens_.emplace_back(line_, TokenType::RPAREN, ")");
          break;
        case '.':
          tokens_.emplace_back(line_, TokenType::DOT, ".");
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
          throw InvalidTokenException{ line_, std::string{ c } };
        }
    }

#ifdef DEBUG
  for (const auto& token : tokens_)
    std::cout << token << "\n";
#endif

  tokens_.push_back(Token{ line_, TokenType::END, "EOF" });
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

  return { line_, TokenType::NUMBER, result };
}

}
