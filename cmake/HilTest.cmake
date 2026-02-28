function(hil_add_runner_test name firmware)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    add_test(
        NAME ${name}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/hil_runner.py
            --firmware ${firmware}
            --test-name ${name}
    )
endfunction()
