cmake_minimum_required(VERSION 3.20)
project(ctm
        VERSION 0.0.1
        LANGUAGES C
)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CTM_UI "tui" CACHE STRING "UI backend to build: tui, gtk, win9x")
set_property(CACHE CTM_UI PROPERTY STRINGS tui gtk win9x)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(Curses REQUIRED)

add_compile_options(
        -Wall                   # Enable most common warnings.
        -Wextra                 # Enable less common warnings not included by -Wall.
        -Wstrict-prototypes     # Require functions to have argument types.
        -pedantic               # Enforce strict ISO C90 compliance.
)

enable_testing()

include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

add_subdirectory(libs/log)
add_subdirectory(libs/collections)

add_executable(${PROJECT_NAME} "src/main.c"
        "src/ctm_cpu_metrics.c"
        "src/ctm_cpu_metrics_internal.h"
        "src/ctm_memory_metrics.c"
        "src/ctm_process_metrics.c"
        "src/ctm_process_manager.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CTM_UI STREQUAL "tui")
    message(STATUS "Building TUI backend")

    set_target_properties(${PROJECT_NAME} PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE CTM_UI_TUI=1)
    target_sources(${PROJECT_NAME} PRIVATE
            "src/tui/ui.c"
            "src/tui/process_tab.c"
    )
    target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES} panel)

elseif (CTM_UI STREQUAL "gtk")
    pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

    message(STATUS "Building GTK backend")
    target_sources(${PROJECT_NAME} PRIVATE
            "src/gtk_ui/ui.c"
            "src/gtk_ui/app_context.c"
            "src/gtk_ui/status_bar.c"
            "src/gtk_ui/process_tab.c")

    set_target_properties(${PROJECT_NAME} PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE CTM_UI_GTK=1)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Threads::Threads
            PkgConfig::GTK3
            log_lib
            collections_lib
            metrics_lib)

elseif (CTM_UI STREQUAL "win9x")
    message(STATUS "Building Win9x backend")
    target_compile_definitions(${PROJECT_NAME} PRIVATE CTM_UI_WIN9X=1)
    target_sources(${PROJECT_NAME} PRIVATE
            "src/win9x_ui/main.c"
            "src/win9x_ui/sound.c"
    )
else ()
    message(FATAL_ERROR "Unknown CTM_UI value: ${CTM_UI}")
endif ()

add_ctm_test(TestCpuMetrics "tests/ctm_cpu_metrics.c"
        "src/ctm_cpu_metrics_internal.h"
        "src/ctm_cpu_metrics.c"
)
add_ctm_test(TestMemoryMetrics "tests/ctm_memory_metrics.c"
        "src/ctm_memory_metrics_internal.h"
        "src/ctm_memory_metrics.c"
)
add_ctm_test(TestProcessMetrics "tests/ctm_process_metrics.c"
        "src/ctm_process_metrics_internal.h"
        "src/ctm_process_metrics.c"
)