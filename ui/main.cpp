#include <curses.h>

#include <cassert>
#include <string>
#include <vector>

#include "mmt.hpp"

static WINDOW* ow;
static WINDOW* iw;

static std::string input{};
static std::vector<std::string> transpiled{};

void
transpile_input()
{
  transpiled.clear();

  auto result{ mmt::transpile(input) };
  int start{ 0 };
  int current{ 0 };

  int rounds{ 0 };

  while (current < result.length())
    {
      assert(rounds < 1000);
      start = current;
      while (current < result.length() && result[current] != '\n')
        current++;
      transpiled.push_back(result.substr(start, current - start - 1));
      current++; // Skip the newline
      rounds++;
    }
}

bool
handle_input()
{
  int c = getch();
  switch (c)
    {
    case 'q':
      return false;
    case '':
      input.pop_back();
      break;
    case '\n':
      transpile_input();
      input.clear();
      break;
    default:
      input += c;
      break;
    }
  return true;
}

auto
loop()
{
  int height, width;
  getmaxyx(stdscr, height, width);

  iw = newwin(height-1, width / 2, 0, 0);
  ow = newwin(height-1, width / 2, 0, width / 2);

  bool keep_running{ true };

  while (keep_running)
    {

      wclear(iw);
      mvwaddstr(iw, 2, 1, input.c_str());
      waddch(iw, '|');

      if (!transpiled.empty())
        {
          wclear(ow);
          wmove(ow, 2, 1);
          int offset{ 0 };
          for (const auto& line : transpiled)
            {
              mvwaddstr(ow, 2 + offset, 1, line.c_str());
              offset++;
            }
        }

      box(iw, '|', '-');
      box(ow, '|', '-');

      mvwaddstr(iw, 0, 1, "Input Window");
      mvwaddstr(ow, 0, 1, "Output Window");

      mvaddstr(height - 1, 1, "(q) Quit (Enter) Transpile");
      refresh();

      wrefresh(iw);
      wrefresh(ow);

      keep_running = handle_input();
    }

  delwin(iw);
  delwin(ow);
}

auto
main() -> int
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  loop();
  endwin();
  return 0;
}
