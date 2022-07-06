#pragma once

/// A span represents a the 'span' that a token occupies in the source code.
class Span
{
public:
  constexpr Span(int s, int e) : start{ s }, end{ e } {}

  /// The start of the span.
  int start = 0;
  /// The end of the span. This is one position after the actual end.
  int end = 0;
};
