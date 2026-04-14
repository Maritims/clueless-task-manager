add_library(metrics STATIC
        src/cpu.c
        src/memory.c
        src/process.c
        src/process_list.c
        src/sampler.c
)

target_include_directories(metrics
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(metrics PRIVATE
        Threads::Threads
        collections
)

add_ctm_test(TestCpu "tests/cpu.c" "src/cpu.c")
target_link_libraries(TestCpu metrics)

add_ctm_test(TestMemory "tests/memory.c" "src/memory.c")
target_link_libraries(TestMemory metrics)