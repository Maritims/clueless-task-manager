function(add_ctm_test TEST_NAME)
    # The first argument is the name, all others are the source files
    set(TEST_SOURCES ${ARGN})

    # We always want the test utility (test.c) included
    set(TEST_UTIL_SRC "tests/test.c")

    # Create the executable
    add_executable(${TEST_NAME} ${TEST_UTIL_SRC} ${TEST_SOURCES})

    # Apply standard include directories to the new target
    target_include_directories(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests
    )

    # Register the test with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Optional: Group them in IDEs like Visual Studio/CLion
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Tests")
endfunction()