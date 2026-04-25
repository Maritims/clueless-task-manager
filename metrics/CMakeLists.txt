add_library(metrics STATIC
        src/ctm_cpu_metrics.c
        src/ctm_memory_metrics.c
        src/ctm_process_manager.c
        src/ctm_process_metrics.c
        src/ctm_sampler.c
)
target_include_directories(metrics PRIVATE src/internal)
target_include_directories(metrics PUBLIC include)
target_link_libraries(metrics PRIVATE core)

CtmAddTest(ctm_cpu_metrics metrics)
CtmAddTest(ctm_memory_metrics metrics)
CtmAddTest(ctm_process_manager metrics)
CtmAddTest(ctm_process_metrics metrics)