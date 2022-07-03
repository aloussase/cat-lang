#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cat.hpp"

#define CAT_TMP_NAME "cat-out.mips"
#define SPIM_EXE "/usr/bin/spim"

/// Write the program to a temporary file and run it using SPIM.
void
execute_program(const std::string& program)
{
  FILE* fp = std::fopen(CAT_TMP_NAME, "w");

  if (!fp)
    {
      std::perror("Failed to create temporary file");
      std::exit(EXIT_FAILURE);
    }

  if (std::fputs(program.c_str(), fp) < 0)
    {
      std::perror("Failed to write program to temporary file");
      std::exit(EXIT_FAILURE);
    }

  if (std::fclose(fp) == EOF)
    {
      std::perror("Failed to close file");
      std::exit(EXIT_FAILURE);
    }

  pid_t pid = fork();
  int pipefd[2];
  if (pipe(pipefd) == -1)
    {
      perror("Failed to pipe");
      std::exit(EXIT_FAILURE);
    }

  if (pid == 0) // Child
    {
      // Close read end
      close(pipefd[0]);

      // Set our write end of the pipe to output stdout
      if (dup2(1, pipefd[1]) == -1)
        {
          std::perror("Failed to duplicate 1");
          std::exit(EXIT_FAILURE);
        }

      char* const argv[] = {
        // Para que no joda el compilador
        (char*)SPIM_EXE,
        (char*)"-f",
        (char*)CAT_TMP_NAME,
        (char*)NULL,
      };

      execvp(SPIM_EXE, argv);

      perror("exec");
      std::exit(EXIT_FAILURE);
    }

  // Close write end
  close(pipefd[1]);

  // Wait for child to exit
  int wstatus;
  if (waitpid(pid, &wstatus, 0) < 0)
    {
      std::perror("Failed to wait for child");
      std::exit(EXIT_FAILURE);
    }

  // Read & output child stdout
  char buf[100];
  while (read(pipefd[0], buf, sizeof(buf)) > 0)
    std::cout << buf;

  // Delete the temporary file
  std::remove(CAT_TMP_NAME);
}

/// Run a repl.
void
repl()
{
  std::string line;

  std::cout << "Welcome to the Cat Language REPL v0.0.1\n";
  std::cout << "Press C-d to exit\n\n";

  std::cout << "> ";
  while (std::getline(std::cin, line))
    {
      if (line == ".quit")
        break;

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

  auto result{ cat::transpile(program, filename) };

  if (!run)
    std::fputs(result.c_str(), fout);
  else
    execute_program(result);

  std::fclose(fout);
  std::fclose(fin);

  return 0;
}
