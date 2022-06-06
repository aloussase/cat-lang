add_executable(mmt-tui
  main.cpp
)

target_link_libraries(mmt-tui PRIVATE mmt)

find_package(Curses)

if(${CURSES_FOUND})
  target_link_libraries(mmt-tui PRIVATE ${CURSES_LIBRARIES})
else()
  message(FATAL_ERROR "ncurses libraries not found")
endif()
