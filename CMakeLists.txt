cmake_minimum_required(VERSION 3.22)
project(dog_test_dt_framework VERSION 0.1.0 LANGUAGES C CXX)

enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(dogtest_core
    framework/src/test_registry.c
    framework/src/runner.c
)

target_include_directories(dogtest_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/framework/include>
        $<INSTALL_INTERFACE:include>
)

add_subdirectory(examples)
add_subdirectory(tests)

# ── Install rules ──────────────────────────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS dogtest_core
    EXPORT dogtest-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY framework/include/dogtest
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES cmake/DogTest.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dogtest
)

install(EXPORT dogtest-targets
    FILE dogtest-targets.cmake
    NAMESPACE dogtest::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dogtest
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/dogtest-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/dogtest-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dogtest
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/dogtest-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dogtest-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dogtest-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dogtest
)
