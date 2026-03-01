add_executable(test_framework test_framework.c)
target_link_libraries(test_framework PRIVATE dogtest_core m)

add_test(
    NAME host_framework_tests
    COMMAND test_framework
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
set_tests_properties(host_framework_tests PROPERTIES
    LABELS "host;unit"
    TIMEOUT 30
)
