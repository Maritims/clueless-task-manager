cmake_minimum_required(VERSION 3.20)
project(clueless_task_mgr
        VERSION 0.0.1
        LANGUAGES C
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
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

set(APP_SOURCES
        "src/io/cio.c"
        "src/sound/sound.c"
        "src/sys/cpu.c"
        "src/sys/memory.c"
        "src/sys/process.c"
        "src/sys/system.c"
        "src/ui/CtmAppContext.c"
        "src/ui/CtmFooter.c"
        "src/ui/CtmProcessPage.c"
        "src/main.c"
)

add_executable(clueless-task-mgr ${APP_SOURCES})

target_include_directories(clueless-task-mgr PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GTK3_INCLUDE_DIRS}
        ${PIPEWIRE_INCLUDE_DIRS}
)

target_compile_options(clueless-task-mgr PRIVATE ${GTK3_CFLAGS_OTHER})

target_link_libraries(clueless-task-mgr PRIVATE
        PkgConfig::GTK3
        PkgConfig::PIPEWIRE
        PkgConfig::SNDFILE
)

enable_testing()

add_ctm_test(TestCpu "tests/sys/test_cpu.c" "src/sys/cpu.c")
add_ctm_test(TestMemory "tests/sys/test_memory.c" "src/sys/memory.c")
add_ctm_test(TestProcess "tests/sys/test_process.c" "src/sys/process.c")
add_ctm_test(TestSystem
        "tests/sys/test_system.c"
        "src/sys/cpu.c"
        "src/sys/memory.c"
        "src/sys/process.c"
        "src/sys/system.c"
)
add_ctm_test(TestSound "tests/sound/test_sound.c" "src/sound/sound.c")
target_link_libraries(TestSound PRIVATE
        PkgConfig::PIPEWIRE
        PkgConfig::SNDFILE
)