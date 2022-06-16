#include <cstdio>
#include <cstring>
#include <iostream>

#include "cat.hpp"

void
repl()
{
  std::string line;

  std::cout << "Welcome to the Cat Language REPL v0.0.1\n";
  std::cout << "Press C-d to exit\n\n";

  std::cout << "> ";
  while (std::getline(std::cin, line))
    {
      std::cout << cat::transpile(line) << "\n";
      std::cout << "> ";
    }
}

static FILE* fin = stdin;
static FILE* fout = stdout;

int
main(int argc, char** argv)
{
  argc--;
  argv++;

  if (argc == 0)
    {
      repl();
      return 0;
    }

  int i{ 0 };
  for (; i < argc; i++)
    {
      if (!std::strcmp(*argv, "-o"))
        {
          fout = fopen(*++argv, "w");
        }
      else if (!std::strcmp(*argv, "-"))
        {
          fin = stdin;
        }
      else
        {
          break;
        }
    }

  auto nargs{ argc - i };
  if (nargs > 0)
    {
      fin = std::fopen(*argv, "r");
    }

  char line[100];
  std::fgets(line, 100, fin);
  std::fputs(cat::transpile(line).c_str(), fout);

  fclose(fout);
  fclose(fin);

  return 0;
}
