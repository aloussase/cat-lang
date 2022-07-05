add_executable(cat-api
    main.cpp
)

target_link_libraries(cat-api PRIVATE Crow::Crow)
target_link_libraries(cat-api PRIVATE cat-lang)

