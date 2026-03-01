add_executable(spi_transfer
    main.c
)

target_link_libraries(spi_transfer
    PRIVATE
        dogtest_core
)
