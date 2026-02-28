cmake_minimum_required(VERSION 3.22)
project(dog_test_hil_framework VERSION 0.1.0 LANGUAGES C CXX)

enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(hiltest_core
    framework/src/test_registry.c
    framework/src/runner.c
)

target_include_directories(hiltest_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/framework/include
)

add_subdirectory(examples/uart_loopback)
add_subdirectory(examples/i2c_smoke)
add_subdirectory(examples/spi_transfer)
add_subdirectory(tests)
add_subdirectory(tests/host)
