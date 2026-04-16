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

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

add_compile_options(
        -Wall                   # Enable most common warnings.
        -Wextra                 # Enable less common warnings not included by -Wall.
        -Wstrict-prototypes     # Require functions to have argument types.
        -pedantic               # Enforce strict ISO C90 compliance.
)

enable_testing()

include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

add_subdirectory(src/collections)
add_subdirectory(src/metrics)

add_executable(${PROJECT_NAME}
        src/main.c
        src/terminal_ui.c
        ${UI_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
        PRIVATE collections
        PRIVATE metrics
)

pkg_check_modules(GTK3 IMPORTED_TARGET gtk+-3.0)

if(GTK3_FOUND)
    add_executable(ctm-gui "src/main.c" "src/gtk_ui.c" ${COMMON_SOURCES} ${LINUX_SOURCES})
    target_compile_definitions(ctm-gui PRIVATE USE_GTK)
    target_include_directories(ctm-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(ctm-gui PRIVATE Threads::Threads PkgConfig::GTK3 collections metrics)
else()
    message(STATUS "GTK3 not found - ctm-gui target will not be built")
endif ()