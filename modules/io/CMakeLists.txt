add_library(cio STATIC
        src/cio.c
)

target_include_directories(cio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)