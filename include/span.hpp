#pragma once

class Span
{
public:
  Span(int s, int e) : start{ s }, end{ e } {}

  int start = 0;
  int end = 0;
};
