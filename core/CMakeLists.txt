add_library(core STATIC
    src/linux/ctm_platform.c
    src/hash_map.c
    src/list.c
    src/log.c
    src/ring_buffer.c
)
target_include_directories(core PUBLIC include)

CtmAddTest(hash_map core)
CtmAddTest(ring_buffer core)