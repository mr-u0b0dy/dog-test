add_executable(spi_transfer
    main.c
)

target_link_libraries(spi_transfer
    PRIVATE
        hiltest_core
)

target_include_directories(spi_transfer PRIVATE ${CMAKE_SOURCE_DIR}/framework/include)
