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
    case TokenType::IDENTIFIER:
      return "identifier";
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
  while (!is_at_end())
    {
      char c{ advance() };
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
          tokens_.push_back(number(c));
          break;
        case ' ':
        case '\t':
          break;
        case '\n':
          line_++;
          break;
        default:
          {
            if (is_identifier_character(c))
              tokens_.push_back(identifier(c));
            else
              throw InvalidTokenException{ line_, std::string{ c } };
          }
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
Lexer::is_at_end() const noexcept
{
  return !(current_ < source_.length());
}

bool
Lexer::is_identifier_character(char c) const noexcept
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'
         || c == '/';
}

char
Lexer::advance() noexcept
{
  return source_[current_++];
}

bool
is_digit(char c)
{
  return c >= '0' && c <= '9';
}

char
Lexer::peek() noexcept
{
  return source_[current_];
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

  return { line_, TokenType::NUMBER, result };
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

  return { line_, TokenType::IDENTIFIER, result };
}

}
