pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

add_library(sound STATIC src/ctm_sound.c
)

target_link_libraries(sound PRIVATE
        cio
)

target_link_libraries(sound PUBLIC
        m
        PkgConfig::PIPEWIRE
        PkgConfig::SNDFILE)

target_include_directories(sound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_sound tests/test_sound.c)
target_link_libraries(test_sound sound)
add_test(NAME SoundTest COMMAND test_sound)