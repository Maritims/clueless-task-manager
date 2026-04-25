add_library(ui STATIC
        src/tui/ui.c
)
target_include_directories(ui PUBLIC include)

# External dependencies
find_package(Curses REQUIRED)
set(CURSES_LIBRARIES ${CURSES_LIBRARIES} panel)

target_link_libraries(ui PRIVATE ${CURSES_LIBRARIES} metrics core)