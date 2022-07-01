#pragma once

#include <string>

#include "span.hpp"

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

  // TODO: Make a NullSpan for hints

  Diagnostic(const std::string& message) : Diagnostic{ 0, message, Span{ 0, 0 } } {}

  Diagnostic(int line, const std::string& message, Span span)
      : Diagnostic{ line, Severity::ERROR, message, span }
  {
  }

  Diagnostic(Severity severity, const std::string& message) : Diagnostic{ 0, severity, message, Span{ 0, 0 } }
  {
  }

  Diagnostic(int line, Severity severity, const std::string& message, Span span)
      : m_line{ line }, m_severity{ severity }, m_message{ message }, m_span{ span }
  {
  }

  /**
   * Format this diagnostic and return the result as a string.
   */
  std::string format(const std::string& file = "<repl>", const std::string& repl_line = "") const noexcept;

  /**
   * Display this diagnostic.
   */
  void show(const std::string& file = "<repl>") const noexcept;

private:
  std::string format_error(const std::string& repl_line) const noexcept;
  std::string format_hint() const noexcept;

  mutable std::string m_file = "<repl>";
  int m_line = {};
  Severity m_severity;
  std::string m_message = {};
  Span m_span;
};

} // namespace cat
