#include "diagnostic.hpp"

#include <iostream>

namespace cat
{

void
Diagnostic::show() const noexcept
{
  if (m_severity == Severity::ERROR)
    show_error();
  else
    show_hint();
}

void
Diagnostic::show_error() const noexcept
{
  std::cout << "\033[31mError:\033[m" << m_line << ": " << m_message << "\n";
}

void
Diagnostic::show_hint() const noexcept
{
  std::cout << "\033[34mHint:\033[m\n\t" << m_message << "\n";
}

}
