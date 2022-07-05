add_executable(cat-api
    main.cpp
)
find_package(Crow)
target_link_libraries(cat-api PUBLIC Crow::Crow)

target_link_libraries(cat-api PRIVATE cat-lang)

