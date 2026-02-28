# ── HilTest.cmake ── re-usable helpers for registering HIL tests ──

# hil_add_runner_test(<name> <firmware>
#   [RESET_MODE none|soft|hard]
#   [TIMEOUT seconds]
#   [LABELS label1 label2 ...]
#   [SKIP_FLASH] [SKIP_RESET] [SKIP_MONITOR] [SKIP_TARGET_EXEC])
#
# Registers a CTest that invokes hil_runner.py with sensible defaults.
function(hil_add_runner_test name firmware)
    cmake_parse_arguments(HIL
        "SKIP_FLASH;SKIP_RESET;SKIP_MONITOR;SKIP_TARGET_EXEC"   # options
        "RESET_MODE;TIMEOUT"                                     # one-value
        "LABELS"                                                 # multi-value
        ${ARGN}
    )

    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    set(_cmd
        ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/hil_runner.py
        --firmware ${firmware}
        --test-name ${name}
    )

    if(HIL_RESET_MODE)
        list(APPEND _cmd --reset-mode ${HIL_RESET_MODE})
    else()
        list(APPEND _cmd --reset-mode none)
    endif()

    if(HIL_SKIP_FLASH)
        list(APPEND _cmd --skip-flash)
    endif()
    if(HIL_SKIP_RESET)
        list(APPEND _cmd --skip-reset)
    endif()
    if(HIL_SKIP_MONITOR)
        list(APPEND _cmd --skip-monitor)
    endif()
    if(HIL_SKIP_TARGET_EXEC)
        list(APPEND _cmd --skip-target-exec)
    endif()

    add_test(NAME ${name} COMMAND ${_cmd})
    set_tests_properties(${name} PROPERTIES
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    if(HIL_TIMEOUT)
        set_tests_properties(${name} PROPERTIES TIMEOUT ${HIL_TIMEOUT})
    else()
        set_tests_properties(${name} PROPERTIES TIMEOUT 120)
    endif()

    if(HIL_LABELS)
        set_tests_properties(${name} PROPERTIES LABELS "${HIL_LABELS}")
    endif()
endfunction()

# hil_add_plan_test(<name> <plan_json>
#   [LABELS label1 label2 ...]
#   [TIMEOUT seconds]
#   [RESET_BETWEEN_TESTS]
#   [SKIP_FLASH] [SKIP_RESET] [SKIP_MONITOR] [SKIP_TARGET_EXEC])
function(hil_add_plan_test name plan_json)
    cmake_parse_arguments(HIL
        "RESET_BETWEEN_TESTS;SKIP_FLASH;SKIP_RESET;SKIP_MONITOR;SKIP_TARGET_EXEC"
        "TIMEOUT"
        "LABELS"
        ${ARGN}
    )

    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    set(_cmd
        ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/hil_runner.py
        --test-plan ${plan_json}
    )

    if(HIL_RESET_BETWEEN_TESTS)
        list(APPEND _cmd --reset-between-tests)
    endif()
    if(HIL_SKIP_FLASH)
        list(APPEND _cmd --skip-flash)
    endif()
    if(HIL_SKIP_RESET)
        list(APPEND _cmd --skip-reset)
    endif()
    if(HIL_SKIP_MONITOR)
        list(APPEND _cmd --skip-monitor)
    endif()
    if(HIL_SKIP_TARGET_EXEC)
        list(APPEND _cmd --skip-target-exec)
    endif()

    add_test(NAME ${name} COMMAND ${_cmd})
    set_tests_properties(${name} PROPERTIES
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    if(HIL_TIMEOUT)
        set_tests_properties(${name} PROPERTIES TIMEOUT ${HIL_TIMEOUT})
    else()
        set_tests_properties(${name} PROPERTIES TIMEOUT 120)
    endif()

    if(HIL_LABELS)
        set_tests_properties(${name} PROPERTIES LABELS "${HIL_LABELS}")
    endif()
endfunction()
