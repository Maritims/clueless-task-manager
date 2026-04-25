cmake_minimum_required(VERSION 3.10)
project(ctm C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ANSI_FLAGS -Wall -Wextra -Werror -Wstrict-prototypes -Wmissing-prototypes -pedantic)
add_compile_options(${ANSI_FLAGS})

enable_testing()

function(CtmAddTest test_name)
    add_executable(${test_name}_test tests/${test_name}.c)
    target_compile_options(${test_name}_test PRIVATE -Wno-missing-prototypes)
    target_include_directories(${test_name}_test PRIVATE application/src/metrics ui/${UI_TYPE})
    target_link_libraries(${test_name}_test PRIVATE testing ${ARGN})
    add_test(NAME ${test_name} COMMAND ${test_name}_test)
endfunction()

function(CtmAddTests)
    foreach(test_name IN LISTS ARGN)
        CtmAddTest(${test_Name})
    endforeach ()
endfunction()

add_subdirectory(core)
add_subdirectory(metrics)
add_subdirectory(ui)

# --- Main Executable ---
add_executable(ctm application/src/main.c)
target_link_libraries(ctm PRIVATE core metrics ui)