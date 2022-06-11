add_executable(mmt-tui
  main.cpp
)

target_link_libraries(mmt-tui PRIVATE mmt)

find_package(Curses REQUIRED)
target_link_libraries(mmt-tui PRIVATE ${CURSES_LIBRARIES})
