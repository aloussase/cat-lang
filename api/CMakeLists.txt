add_executable(cat-api
    main.cpp
)

target_link_libraries(cat-api PRIVATE cat-lang pthread)
