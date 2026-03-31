# -----------------------------------------------
# Source files
# -----------------------------------------------
add_library(sysinfo STATIC
        src/ctm_cpu.c
        src/ctm_memory.c
        src/ctm_process.c
        src/ctm_system.c
)

# -----------------------------------------------
# Include directories
# -----------------------------------------------
target_include_directories(sysinfo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_cpu tests/test_cpu.c)
target_link_libraries(test_cpu sysinfo)
add_test(NAME CpuTest COMMAND test_cpu)

add_executable(test_memory tests/test_memory.c)
target_link_libraries(test_memory sysinfo)
add_test(NAME MemoryTest COMMAND test_memory)

add_executable(test_process tests/test_process.c)
target_link_libraries(test_process sysinfo)
add_test(NAME ProcessTest COMMAND test_process)

add_executable(test_system tests/test_system.c)
target_link_libraries(test_system sysinfo)
add_test(NAME SystemTest COMMAND test_system)