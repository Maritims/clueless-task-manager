add_library(testing STATIC
        src/test.c
)
target_include_directories(testing PUBLIC include)