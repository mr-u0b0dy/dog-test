add_executable(i2c_smoke
    main.c
)

target_link_libraries(i2c_smoke
    PRIVATE
        hiltest_core
)
