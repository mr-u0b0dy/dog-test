add_executable(spi_transfer
    main.c
)

target_link_libraries(spi_transfer
    PRIVATE
        hiltest_core
)
