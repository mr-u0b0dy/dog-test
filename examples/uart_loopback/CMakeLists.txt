add_executable(uart_loopback
    main.c
)

target_link_libraries(uart_loopback
    PRIVATE
        hiltest_core
)

target_include_directories(uart_loopback PRIVATE ${CMAKE_SOURCE_DIR}/framework/include)
