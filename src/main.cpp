#include <cstdio>
#include <cstring>
#include <iostream>

#include "cat.hpp"

/// Run a repl.
void
repl()
{
  std::string line;

  std::cout << "Welcome to the Cat Language REPL v0.0.1\n";
  std::cout << "Press C-d to exit\n\n";

  std::cout << "> ";

  std::string result;

  while (std::getline(std::cin, line))
    {
      if (line == ".quit")
        break;

      cat::transpile(line, result);
      std::cout << result << "\n> ";
      result.clear();
    }
}

static FILE* fin = stdin;
static FILE* fout = stdout;

int
main(int argc, char** argv)
{
  argc--;
  argv++;

  std::string filename{};
  bool run = false;

  if (argc == 0)
    {
      repl();
      return 0;
    }

  int i{ 0 };
  for (; i < argc; i++)
    {
      if (!std::strcmp(*argv, "-o"))
        fout = fopen(*++argv, "w");
      else if (!std::strcmp(*argv, "-"))
        {
          filename = "stdin";
          fin = stdin;
          argv++;
        }
      else if (!std::strcmp(*argv, "--run"))
        {
          run = true;
          argv++;
        }
      else
        break;
    }

  if (auto nargs{ argc - i }; nargs > 0)
    {
      filename = *argv;
      fin = std::fopen(filename.c_str(), "r");
    }

  char line[4096];
  std::string program{};

  while (std::fgets(line, sizeof(line), fin) != NULL)
    program += line;

  std::string result{};
  auto ok{ cat::transpile(program, result, filename) };

  if (!run)
    std::fputs(result.c_str(), fout);
  else if (ok)
    // We need to check if there were any errors before sending the
    // transpiler's output to SPIM.
    std::cout << cat::execute(result);
  else
    std::cout << result;

  std::fclose(fout);
  std::fclose(fin);

  return 0;
}
