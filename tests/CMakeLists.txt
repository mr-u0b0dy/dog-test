include(${CMAKE_SOURCE_DIR}/cmake/HilTest.cmake)

# ── Per-example HIL tests ──────────────────────────────────────────────

hil_add_runner_test(hil_uart_example
    ${CMAKE_BINARY_DIR}/examples/uart_loopback/uart_loopback
    RESET_MODE none
    LABELS hil uart
    SKIP_FLASH SKIP_RESET SKIP_MONITOR SKIP_TARGET_EXEC
)

hil_add_runner_test(hil_i2c_example
    ${CMAKE_BINARY_DIR}/examples/i2c_smoke/i2c_smoke
    RESET_MODE soft
    LABELS hil i2c
    SKIP_FLASH SKIP_RESET SKIP_MONITOR SKIP_TARGET_EXEC
)

hil_add_runner_test(hil_spi_example
    ${CMAKE_BINARY_DIR}/examples/spi_transfer/spi_transfer
    RESET_MODE hard
    LABELS hil spi
    SKIP_FLASH SKIP_RESET SKIP_MONITOR SKIP_TARGET_EXEC
)

# ── Test-plan driven smoke test ────────────────────────────────────────

hil_add_plan_test(hil_plan_smoke
    ${CMAKE_SOURCE_DIR}/tests/test_plan_smoke.json
    RESET_BETWEEN_TESTS
    LABELS hil plan
    SKIP_FLASH SKIP_RESET SKIP_MONITOR SKIP_TARGET_EXEC
)

# ── Python unit tests ──────────────────────────────────────────────────

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(
    NAME python_unit_tests
    COMMAND ${Python3_EXECUTABLE} -m pytest
            ${CMAKE_SOURCE_DIR}/tests/python/ -v --tb=short
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(python_unit_tests PROPERTIES
    LABELS "host;python;unit"
    TIMEOUT 60
)
