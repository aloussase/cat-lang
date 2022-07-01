#include "diagnostic.hpp"

#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <vector>

namespace cat
{

std::vector<Span> line_spans(const std::string&);

std::string
Diagnostic::format(const std::string& file, const std::string& repl_line) const noexcept
{
  m_file = file;

  if (m_severity == Severity::ERROR)
    return format_error(repl_line);
  return format_hint();
}

void
Diagnostic::show(const std::string& file) const noexcept
{
  std::cout << format(file);
}

std::string
Diagnostic::format_error(const std::string& repl_line) const noexcept
{
  std::string error{};

  // Load the file contents into a string
  std::string file_contents{};

  if (m_file != "<repl>")
    {
      std::ifstream ifs(m_file);
      file_contents
          = std::string{ (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()) };
    }
  else
    {
      file_contents = repl_line;
    }

  error += "\033[31merror:\033[m " + m_message + "\n";

  // Copied this part from Jakt xD
  auto spans{ line_spans(file_contents) };

  if (spans.size() < 1)
    error += "\n";

  for (auto current_line_span{ spans.begin() }; current_line_span != spans.end(); current_line_span++)
    {
      if (m_span.start >= current_line_span->start && m_span.end <= current_line_span->end)
        {
          auto line_index{ std::distance(spans.begin(), current_line_span) };
          auto column_index{ m_span.start - current_line_span->start };

          // Add filename:line:col
          error += "  \033[34m-->\033[m " + m_file + ":" + std::to_string(m_line) + ":"
                   + std::to_string(column_index + 1) + "\n\n";

          if (line_index < spans.size() && m_span.end > current_line_span->start)
            {
              //     1 |
              error += "    \033[31m" + std::to_string(line_index + 1) + " | \033[m";

              // Add the source line
              while (line_index < spans.size() && m_span.end > current_line_span->start)
                error += file_contents[current_line_span->start++];
              error += "\n";

              // Add a caret showing the error position
              for (int i{ 1 }; i < m_span.end + 8; i++)
                error += " ";
              error += "\033[31m^\033[m\n";

              // NOTE: This is not currently used because we are only displaying a single line of
              // source code error.
              line_index++;
            }
        }
    }

  return error;
}

std::string
Diagnostic::format_hint() const noexcept
{
  return "\033[34mhint:\033[m\n  \033[34m-->\033[m " + m_message + "\n\n";
}

std::vector<Span>
line_spans(const std::string& contents)
{
  std::string line;

  std::vector<Span> spans{};
  int start{};
  int current{};

  for (const auto& c : contents)
    {
      if (c == '\n')
        {
          spans.push_back({ start, current });
          start = current + 1;
        }
      current++;
    }

  spans.push_back({ start, current });
  return spans;
}

}
