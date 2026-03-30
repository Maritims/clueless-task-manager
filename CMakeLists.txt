cmake_minimum_required(VERSION 3.20)
project(clueless_task_mgr
        VERSION     0.0.1
        LANGUAGES   C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_compile_options(
        -Wall                   # Enable most common warnings.
        -Wextra                 # Enable less common warnings not included by -Wall.
        -Wstrict-prototypes     # Require functions to have argument types.
        -pedantic               # Enforce strict ISO C90 compliance.
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_subdirectory(modules/sysinfo)

file(GLOB_RECURSE APP_SOURCES
        "src/ctm_process_page.c"
        "src/main.c"
)

add_executable(clueless-task-mgr ${APP_SOURCES}
        include/ctm_common.h)

target_include_directories(clueless-task-mgr PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GTK3_INCLUDE_DIRS}
)

target_compile_options(clueless-task-mgr PRIVATE ${GTK3_CFLAGS_OTHER})

target_link_directories(clueless-task-mgr PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(clueless-task-mgr PRIVATE sysinfo ${GTK3_LIBRARIES})

enable_testing()