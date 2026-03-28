# -----------------------------------------------
# Source files
# -----------------------------------------------
add_library(sysinfo STATIC
        src/cpu.c
        src/memory.c
        src/process.c
        src/sysinfo.c
)

# -----------------------------------------------
# Include directories
# -----------------------------------------------
target_include_directories(sysinfo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_cpu tests/test_cpu.c)
target_link_libraries(test_cpu sysinfo)
add_test(NAME CpuTest COMMAND test_cpu)

add_executable(test_process tests/test_process.c)
target_link_libraries(test_process sysinfo)
add_test(NAME ProcessTest COMMAND test_process)