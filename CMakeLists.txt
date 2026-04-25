cmake_minimum_required(VERSION 3.10)

project(ctm C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ANSI_FLAGS -Wall -Wextra -Werror -Wstrict-prototypes -Wmissing-prototypes -pedantic)
add_compile_options(${ANSI_FLAGS})

# External dependencies
find_package(Curses REQUIRED)
set(CURSES_LIBRARIES ${CURSES_LIBRARIES} panel)

# --- Core Library ---
add_library(core STATIC
        src/core/hash_map.c
        src/core/list.c
        src/core/log.c
        src/core/ring_buffer.c
        src/core/linux/ctm_platform.c
)
target_include_directories(core PUBLIC include)

# --- Metrics Library ---
add_library(metrics STATIC
        src/metrics/ctm_cpu_metrics.c
        src/metrics/ctm_memory_metrics.c
        src/metrics/ctm_process_metrics.c
        src/metrics/ctm_process_manager.c
        src/metrics/ctm_sampler.c
)
target_include_directories(metrics PUBLIC include src/metrics)

# --- UI Library ---
set(UI_TYPE "tui" CACHE STRING "UI type (tui, gtk, win9x_ui)")
# Manually list UI files or use a specific logic based on UI_TYPE
add_library(ui STATIC
        src/ui/${UI_TYPE}/ui.c
        src/ui/${UI_TYPE}/ctm_process_tab.c
)
target_include_directories(ui PUBLIC include src/ui/${UI_TYPE})
target_link_libraries(ui PRIVATE metrics core ${CURSES_LIBRARIES} panel)

# --- Main Executable ---
add_executable(ctm src/main.c)
target_link_libraries(ctm PRIVATE ui)

# --- Tests ---
enable_testing()
add_library(test_common STATIC tests/test.c)
target_include_directories(test_common PUBLIC tests)

set(UNIT_TESTS
        ctm_cpu_metrics
        ctm_memory_metrics
        ctm_process_metrics
        ctm_process_manager
        hash_map
        ring_buffer
)
foreach(test_name ${UNIT_TESTS})
    add_executable(${test_name}_test tests/${test_name}.c)
    target_compile_options(${test_name}_test PRIVATE -Wno-missing-prototypes)
    target_link_libraries(${test_name}_test PRIVATE test_common ui metrics)
    add_test(NAME ${test_name} COMMAND ${test_name}_test)
endforeach ()