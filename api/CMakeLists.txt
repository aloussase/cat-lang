add_executable(cat-api
    main.cpp
)

target_link_libraries(cat-api PRIVATE cat-lang)

target_include_directories(cat-api PRIVATE ${CROW_DIR}/include)
target_link_libraries(cat-api PRIVATE Crow::Crow)

