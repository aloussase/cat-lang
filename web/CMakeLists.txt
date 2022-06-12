add_executable(web
    main.cpp
)

target_link_libraries(web PRIVATE mmt pthread)
