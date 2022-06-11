add_executable(web
    main.cpp
)

target_link_libraries(web PRIVATE mmt)

find_package(Crow REQUIRED)
target_link_libraries(web PRIVATE Crow::Crow)

