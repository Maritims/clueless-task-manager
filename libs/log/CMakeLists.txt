add_library(log_lib STATIC
        src/log.c
)

set_target_properties(log_lib PROPERTIES
        C_STANDARD 90
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
)

target_include_directories(log_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        src/ # Private headers only used for building the library
)