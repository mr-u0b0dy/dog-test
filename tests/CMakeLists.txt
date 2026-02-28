find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(HIL_RUNNER ${CMAKE_SOURCE_DIR}/tools/hil_runner.py)

add_test(
    NAME hil_uart_example
    COMMAND ${Python3_EXECUTABLE} ${HIL_RUNNER}
        --firmware ${CMAKE_BINARY_DIR}/examples/uart_loopback/uart_loopback
        --test-name uart_loopback
        --reset-mode none
    --skip-flash
        --skip-reset
    --skip-monitor
    --skip-target-exec
)

set_tests_properties(hil_uart_example PROPERTIES LABELS "hil;uart")

add_test(
    NAME hil_i2c_example
    COMMAND ${Python3_EXECUTABLE} ${HIL_RUNNER}
        --firmware ${CMAKE_BINARY_DIR}/examples/i2c_smoke/i2c_smoke
        --test-name i2c_smoke
        --reset-mode soft
        --skip-flash
        --skip-reset
        --skip-monitor
        --skip-target-exec
)

set_tests_properties(hil_i2c_example PROPERTIES LABELS "hil;i2c")

add_test(
    NAME hil_spi_example
    COMMAND ${Python3_EXECUTABLE} ${HIL_RUNNER}
        --firmware ${CMAKE_BINARY_DIR}/examples/spi_transfer/spi_transfer
        --test-name spi_transfer
        --reset-mode hard
        --skip-flash
        --skip-reset
        --skip-monitor
        --skip-target-exec
)

set_tests_properties(hil_spi_example PROPERTIES LABELS "hil;spi")

add_test(
    NAME hil_plan_smoke
    COMMAND ${Python3_EXECUTABLE} ${HIL_RUNNER}
        --test-plan ${CMAKE_SOURCE_DIR}/tests/test_plan_smoke.json
        --reset-between-tests
        --skip-flash
        --skip-reset
        --skip-monitor
        --skip-target-exec
)

set_tests_properties(hil_plan_smoke PROPERTIES LABELS "hil;plan")
