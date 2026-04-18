add_library(collections STATIC
        src/array.c
        src/array_iter.c
        src/hash_map.c
        src/ring_buffer.c
)

target_include_directories(collections PUBLIC include)

target_link_libraries(collections PRIVATE logging)

add_ctm_test(TestArray "tests/array.c" "src/array.c")
target_link_libraries(TestArray collections)

add_ctm_test(TestArrayIter "tests/array_iter.c" "src/array_iter.c")
target_link_libraries(TestArrayIter collections)

add_ctm_test(TestHashMap "tests/hash_map.c" "src/hash_map.c")
target_link_libraries(TestHashMap collections)

add_ctm_test(TestRingBuffer "tests/ring_buffer.c" "src/ring_buffer.c")
target_link_libraries(TestRingBuffer collections)