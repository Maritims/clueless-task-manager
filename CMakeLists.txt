cmake_minimum_required(VERSION 3.20)
project(clueless_task_mgr C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find the GTK package.
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include local headers and GTK headers.
include_directories(include ${GTK3_INCLUDE_DIRS})

# Gather source files (.c).
file(GLOB_RECURSE SOURCES "src/*.c")

# Define the executable
add_executable(clueless_task_mgr ${SOURCES}
        src/ui.c
        include/ui.h
        include/callbacks.h
        src/callbacks.c)

# Link the GTK libraries.
target_link_libraries(clueless_task_mgr ${GTK3_LIBRARIES})

# Add the GTK compiler flags.
add_definitions(${GTK3_CFLAGS_OTHER})

enable_testing()

add_executable(run_tests tests/test_main.c src/cpu.c src/memory.c src/sys_stats.c)

target_link_libraries(run_tests ${GTK3_LIBRARIES})
add_test(NAME AllStatsTests COMMAND run_tests)