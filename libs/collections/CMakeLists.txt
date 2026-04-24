add_library(collections_lib STATIC
        src/array.c
        src/array_iter.c
        src/hash_map.c
        src/ring_buffer.c
)

set_target_properties(collections_lib PROPERTIES
        C_STANDARD 90
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
)

target_include_directories(collections_lib PUBLIC include)

target_link_libraries(collections_lib PRIVATE log_lib)

add_ctm_test(TestArray "tests/array.c")
target_link_libraries(TestArray collections_lib)

add_ctm_test(TestArrayIter "tests/array_iter.c")
target_link_libraries(TestArrayIter collections_lib)

add_ctm_test(TestHashMap "tests/hash_map.c")
target_link_libraries(TestHashMap collections_lib)

add_ctm_test(TestRingBuffer "tests/ring_buffer.c")
target_link_libraries(TestRingBuffer collections_lib)