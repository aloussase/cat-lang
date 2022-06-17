#pragma once

#include <string>

namespace cat
{

class Diagnostic final
{
public:
  enum class Severity
  {
    ERROR,
    HINT
  };

  Diagnostic(int line, const std::string& message) : Diagnostic{ line, Severity::ERROR, message } {}

  Diagnostic(int line, Severity severity, const std::string& message)
      : m_line{ line }, m_severity{ severity }, m_message{ message }
  {
  }

  /**
   * Display this diagnostic.
   */
  void show() const noexcept;

private:
  void show_error() const noexcept;
  void show_hint() const noexcept;

  int m_line = {};
  Severity m_severity;
  std::string m_message = {};
};

} // namespace cat
