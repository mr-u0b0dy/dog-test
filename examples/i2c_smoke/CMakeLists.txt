add_executable(i2c_smoke
    main.c
)

target_link_libraries(i2c_smoke
    PRIVATE
        hiltest_core
)

target_include_directories(i2c_smoke PRIVATE ${CMAKE_SOURCE_DIR}/framework/include)
