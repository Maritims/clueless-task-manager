cmake_minimum_required(VERSION 3.20)
project(clueless_task_mgr
        VERSION     0.0.1
        LANGUAGES   C
)

# -----------------------------------------------
# Compiler settings
# -----------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


# Enable warnings and treat them as errors.
add_compile_options(
        -Wall                   # Enable most common warnings.
        -Wextra                 # Enable less common warnings not included by -Wall.
        -Wstrict-prototypes     # Require functions to have argument types.
        -pedantic               # Enforce strict ISO C90 compliance.
)

# -----------------------------------------------
# Include directories
# -----------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/include ${GTK3_INCLUDE_DIRS})

# -----------------------------------------------
# Modules
# -----------------------------------------------
add_subdirectory(modules/sysinfo)

# -----------------------------------------------
# Source files
# -----------------------------------------------
file(GLOB_RECURSE APP_SOURCES
        "src/callbacks.c"
        "src/main.c"
        "src/style.c"
        "src/ui.c"
)

# -----------------------------------------------
# Dependencies
# -----------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# -----------------------------------------------
# Build application
# -----------------------------------------------
add_executable(clueless-task-mgr ${APP_SOURCES})

# Link libraries
target_link_libraries(clueless-task-mgr PRIVATE sysinfo ${GTK3_LIBRARIES})

# Enable testing
enable_testing()