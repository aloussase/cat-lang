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

  Diagnostic(const std::string& message) : Diagnostic{ 0, message } {}

  Diagnostic(int line, const std::string& message) : Diagnostic{ line, Severity::ERROR, message } {}

  Diagnostic(Severity severity, const std::string& message) : Diagnostic{ 0, severity, message } {}

  Diagnostic(int line, Severity severity, const std::string& message)
      : m_line{ line }, m_severity{ severity }, m_message{ message }
  {
  }

  /**
   * Display this diagnostic.
   */
  void show(const std::string& file = "<repl>") const noexcept;

private:
  void show_error() const noexcept;
  void show_hint() const noexcept;

  mutable std::string m_file = "<repl>";
  int m_line = {};
  Severity m_severity;
  std::string m_message = {};
};

} // namespace cat
