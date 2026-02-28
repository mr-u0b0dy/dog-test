add_executable(uart_loopback
    main.c
)

target_link_libraries(uart_loopback
    PRIVATE
        hiltest_core
)
