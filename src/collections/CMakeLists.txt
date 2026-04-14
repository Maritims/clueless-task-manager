add_library(collections STATIC src/array.c src/hash_map.c src/ring_buffer.c)

target_include_directories(collections PUBLIC include)

add_ctm_test(TestHashMap "tests/hash_map.c" "src/hash_map.c")
target_link_libraries(TestHashMap collections)

add_ctm_test(TestRingBuffer "tests/ring_buffer.c" "src/ring_buffer.c")
target_link_libraries(TestRingBuffer collections)