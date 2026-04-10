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

include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

set(COMMON_SOURCES "src/ring_buffer.c" "src/hash_map.c")

set(LINUX_SOURCES
        "src/linux/cpu.c"
        "src/linux/memory.c"
        "src/linux/process.c"
        "src/linux/process_list.c"
        "src/linux/sampler.c"
)

add_executable(${PROJECT_NAME} "src/main.c" "src/terminal_ui.c" ${COMMON_SOURCES} ${LINUX_SOURCES} ${UI_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

pkg_check_modules(GTK3 IMPORTED_TARGET gtk+-3.0)

if(GTK3_FOUND)
    add_executable(ctm-gui "src/main.c" "src/gtk_ui.c" ${COMMON_SOURCES} ${LINUX_SOURCES})
    target_compile_definitions(ctm-gui PRIVATE USE_GTK)
    target_include_directories(ctm-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(ctm-gui PRIVATE Threads::Threads PkgConfig::GTK3)
else()
    message(STATUS "GTK3 not found - ctm-gui target will not be built")
endif ()

enable_testing()

add_ctm_test(TestHashMap "tests/hash_map.c" "src/hash_map.c")
add_ctm_test(TestRingBuffer "tests/ring_buffer.c" "src/ring_buffer.c")
add_ctm_test(TestCpu "tests/linux/cpu.c" "src/linux/cpu.c")
add_ctm_test(TestMemory "tests/linux/memory.c" "src/linux/memory.c")