#include "diagnostic.hpp"

#include <iostream>

namespace cat
{

void
Diagnostic::show(const std::string& file) const noexcept
{
  m_file = file;

  if (m_severity == Severity::ERROR)
    show_error();
  else
    show_hint();
}

void
Diagnostic::show_error() const noexcept
{
  std::cout << "\033[31merror:\033[m " << m_message << "\n";
  std::cout << "  \033[34m-->\033[m " << m_file << ":" << m_line << "\n\n";
}

void
Diagnostic::show_hint() const noexcept
{
  std::cout << "\033[34mhint:\033[m\n  \033[34m-->\033[m " << m_message << "\n\n";
}

}
