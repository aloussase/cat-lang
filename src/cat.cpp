#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Lexer.hpp"
#include "MIPSTranspiler.hpp"
#include "Parser.hpp"
#include "cat.hpp"

#define CAT_TMP_NAME "cat-out.mips"
#define SPIM_EXE "/usr/bin/spim"
#define BUFSIZE  100

namespace cat
{

// TODO: Replace exit with return.
std::string
execute(const std::string& program)
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

  int pipefd[2];
  if (pipe(pipefd) == -1)
    {
      perror("Failed to pipe");
      std::exit(EXIT_FAILURE);
    }

  pid_t pid = fork();
  if (pid == -1)
    {
      perror("fork");
      std::exit(EXIT_FAILURE);
    }

  if (pid == 0) // Child
    {
      // Set our write end of the pipe to output stdout
      if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
          std::perror("Failed to duplicate stdout");
          std::exit(EXIT_FAILURE);
        }

      if (dup2(pipefd[1], STDERR_FILENO) == -1)
        {
          std::perror("Failed to duplicate stdout");
          std::exit(EXIT_FAILURE);
        }

      close(pipefd[0]);
      close(pipefd[1]);

      char* const argv[] = {
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

  char buf[BUFSIZE+1];
  std::string output{};

  int nread{};
  while ((nread = read(pipefd[0], buf, sizeof(BUFSIZE))) > 0)
    {
      buf[nread] = '\0';
      output += buf;
    }

  // Wait for child to exit
  int wstatus;
  if (waitpid(pid, &wstatus, 0) < 0)
    {
      std::perror("Failed to wait for child");
      std::exit(EXIT_FAILURE);
    }

  // Delete the temporary file
  std::remove(CAT_TMP_NAME);

  return output;
}

bool
transpile(const std::string& source, std::string& result, const std::string& file)
{
  std::vector<cat::Diagnostic> diagnostics{};

  auto tokens{ Lexer{ source, diagnostics }.Lex() };

#ifdef DEBUG
  std::cout << "lexer finished\n";
  for (const auto& token : tokens)
    std::cout << token << "\n";
#endif

  auto program{ Parser(tokens, diagnostics).Parse() };

#ifdef DEBUG
  std::cout << "parser finished\n";
#endif

  result = MIPSTranspiler(std::move(program), diagnostics).Transpile();

#ifdef DEBUG
  std::cout << "transpiler finished\n";
#endif

  if (diagnostics.size() == 0)
    return true;

  result.clear();

  for (const auto& diagnostic : diagnostics)
    result += diagnostic.format(file, source);

  return false;
}

}
