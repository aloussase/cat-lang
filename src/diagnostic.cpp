#include "diagnostic.hpp"

#include <iostream>

namespace cat
{

std::string
Diagnostic::format(const std::string& file) const noexcept
{
  m_file = file;

  if (m_severity == Severity::ERROR)
    return format_error();
  return format_hint();
}

void
Diagnostic::show(const std::string& file) const noexcept
{
  std::cout << format(file);
}

std::string
Diagnostic::format_error() const noexcept
{
  std::string error{};
  error += "\033[31merror:\033[m " + m_message + "\n";
  error += "  \033[34m-->\033[m " + m_file + ":" + std::to_string(m_line) + "\n\n";
  return error;
}

std::string
Diagnostic::format_hint() const noexcept
{
  return "\033[34mhint:\033[m\n  \033[34m-->\033[m " + m_message + "\n\n";
}

}
